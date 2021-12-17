int main(){
    /*at&t: opcode source dest*/
    asm(
        "lea (%rsp), %ax \n\t"

        "lea (%edx), %eax \n\t"
        "lea (%edx), %edx \n\t"
        "lea (%edx), %ebx \n\t"

        "lea (%rsp), %rax \n\t"
        ".byte 0x67\n\t"
        "lea 0xff2ff(%ebp,%ebp,8), %edx \n\t"

        "lea (%rip), %rbx \n\t"

        "lea 0x5(%rsp), %rax \n\t"
        "lea 4(%rbp), %rdx \n\t"
    );
    return 0;
}


void load_ireg_from_ir1_addrx(IR1_OPND *opnd1, IR2_OPND value_opnd)
{
#ifdef TARGET_X86_64
    IR2_OPCODE shift_opcode = LISA_SLLI_D;
    IR2_OPCODE shift_opcode_alsl = LISA_ALSL_D;
    /* use for mark the GPR bits, for identify which is 64/32 bits */
    IR1_INST *pir1 = lsenv->tr_data->curr_ir1_inst;
    int bit_size = ir1_addr_size(pir1);

    /* in x64, RIP is used */
    if (ir1_opnd_base_reg(opnd1) == X86_REG_RIP) {
        /* offset = next_IP + offset */
        lsassert(pir1 != NULL);
        int64_t offset = ir1_addr_next(pir1) + ir1_opnd_simm(opnd1);

        load_ireg_from_imm64(value_opnd, offset);
        return;
    }
#else
    IR2_OPCODE shift_opcode = LISA_SLLI_W;
    IR2_OPCODE shift_opcode_alsl = LISA_ALSL_W;
#endif

    longx offset = ir1_opnd_simm(opnd1);
    int16 offset_imm_part;
    longx offset_reg_part;
    IR2_OPND offset_reg_opnd = ra_alloc_itemp();

    if (!(offset >= -2048 && offset <= 2047)){
        offset_imm_part = offset & 0x7FF;
        offset_reg_part = offset - offset_imm_part;
    } else {
        offset_imm_part = offset;
        offset_reg_part = offset - offset_imm_part;
    }

    /* 1. has no index */
    if (!ir1_opnd_has_index(opnd1)) { /* no index and scale */
        if (ir1_opnd_has_base(opnd1)) {
            /* 1.1. base and an optional offset */
            IR2_OPND base_opnd = ra_alloc_gpr(ir1_opnd_base_reg_num(opnd1));
            if (offset_reg_part == 0) {
                la_append_ir2_opnd2i_em(LISA_ADDI_ADDRX, value_opnd, base_opnd,
                                  offset_imm_part);
            } else {
                load_ireg_from_imm64(offset_reg_opnd, offset_reg_part);
                la_append_ir2_opnd3_em(LISA_ADD_ADDRX, value_opnd, base_opnd,
                                 offset_reg_opnd);
                if (offset_imm_part != 0)
                    la_append_ir2_opnd2i_em(LISA_ADDI_ADDRX, value_opnd, value_opnd,
                                      offset_imm_part);
            }
        } else {
            /* only an offset, we should load imm32 */
            load_ireg_from_imm64(value_opnd, offset);
        }
    }

    /* 2. base, index, and an optional offset */
    else if (ir1_opnd_has_base(opnd1)) {
        IR2_OPND base_opnd = ra_alloc_gpr(ir1_opnd_base_reg_num(opnd1));
        IR2_OPND index_opnd = ra_alloc_gpr(ir1_opnd_index_reg_num(opnd1));
        IR2_OPND tmp_opnd = ra_alloc_itemp_internal();

        /* 2.1. prepare base + index*scale */
        // if (ir1_opnd_scale(opnd1) == 1) {
        //     la_append_ir2_opnd3_em(LISA_ADD_ADDRX, value_opnd, base_opnd, index_opnd);
        // } else if (ir1_opnd_scale(opnd1) == 2) {
        //     la_append_ir2_opnd2i_em(shift_opcode, tmp_opnd, index_opnd, 1);
        //     la_append_ir2_opnd3_em(LISA_ADD_ADDRX, value_opnd, tmp_opnd, base_opnd);
        // } else if (ir1_opnd_scale(opnd1) == 4) {
        //     la_append_ir2_opnd2i_em(shift_opcode, tmp_opnd, index_opnd, 2);
        //     la_append_ir2_opnd3_em(LISA_ADD_ADDRX, value_opnd, tmp_opnd, base_opnd);
        // } else if (ir1_opnd_scale(opnd1) == 8) {
        //     la_append_ir2_opnd2i_em(shift_opcode, tmp_opnd, index_opnd, 3);
        //     la_append_ir2_opnd3_em(LISA_ADD_ADDRX, value_opnd, tmp_opnd, base_opnd);
        // } else {
        //     lsassert(0);
        // }
        /****/
        if (ir1_opnd_scale(opnd1) == 1) {
            la_append_ir2_opnd3_em(LISA_ADD_ADDRX, value_opnd, base_opnd, index_opnd);
        } else if (ir1_opnd_scale(opnd1) == 2) {
            la_append_ir2_opnd3i(shift_opcode_alsl, value_opnd, index_opnd, base_opnd, 0);
        } else if (ir1_opnd_scale(opnd1) == 4) {
            la_append_ir2_opnd3i(shift_opcode_alsl, value_opnd, index_opnd, base_opnd, 1);
        } else if (ir1_opnd_scale(opnd1) == 8) {
            la_append_ir2_opnd3i(shift_opcode_alsl, value_opnd, index_opnd, base_opnd, 2);
        } else {
            lsassert(0);
        }
        ra_free_temp(tmp_opnd);

        if (offset != 0) {
            if (offset_reg_part == 0) { /* offset_imm_part != 0 */
                la_append_ir2_opnd2i_em(LISA_ADDI_ADDRX, value_opnd, value_opnd,
                                  offset_imm_part);
            } else {
                load_ireg_from_imm64(offset_reg_opnd, offset_reg_part);
                la_append_ir2_opnd3_em(LISA_ADD_ADDRX, value_opnd, value_opnd,
                                 offset_reg_opnd);
                if (offset_imm_part != 0)
                    la_append_ir2_opnd2i_em(LISA_ADDI_ADDRX, value_opnd, value_opnd,
                                      offset_imm_part);
            }
        }

    }

    /* 3. index and an optional offset */
    else {
        IR2_OPND index_opnd = ra_alloc_gpr(ir1_opnd_index_reg_num(opnd1));

        /* 2.1. prepare index*scale */
        if (ir1_opnd_scale(opnd1) == 1) {
            la_append_ir2_opnd2i_em(shift_opcode, value_opnd, index_opnd, 0);
        } else if (ir1_opnd_scale(opnd1) == 2) {
            la_append_ir2_opnd2i_em(shift_opcode, value_opnd, index_opnd, 1);
        } else if (ir1_opnd_scale(opnd1) == 4) {
            la_append_ir2_opnd2i_em(shift_opcode, value_opnd, index_opnd, 2);
        } else if (ir1_opnd_scale(opnd1) == 8) {
            la_append_ir2_opnd2i_em(shift_opcode, value_opnd, index_opnd, 3);
        } else {
            lsassert(0);
        }

        if (offset != 0) {
            if (offset_reg_part == 0) {
                la_append_ir2_opnd2i_em(LISA_ADDI_ADDRX, value_opnd, value_opnd,
                                  offset_imm_part);
            } else {
                load_ireg_from_imm64(offset_reg_opnd, offset_reg_part);
                la_append_ir2_opnd3_em(LISA_ADD_ADDRX, value_opnd, value_opnd,
                                 offset_reg_opnd);
                if (offset_imm_part != 0)
                    la_append_ir2_opnd2i_em(LISA_ADDI_ADDRX, value_opnd, value_opnd,
                                      offset_imm_part);
            }
        }
    }

    /* 4. segment? */
    if (ir1_opnd_has_seg(opnd1)) {
        lsassertm(0, "not implemented in %s\n", __FUNCTION__);
        /* IR2_OPND seg_base_opnd = ra_alloc_itemp_internal(); */
        /* append_ir2_opnd2i(mips_load_addrx, seg_base_opnd, env_ir2_opnd, */
        /* env->offset_of_seg_base(ir1_opnd_get_seg_index(opnd1);)); */
        /* append_ir2_opnd3(mips_add_addrx, value_opnd, seg_base_opnd, */
        /* value_opnd); */
    }
#ifdef TARGET_X86_64
    /* 5. strip the regs*/
    if (bit_size == 32) {
        la_append_ir2_opnd2_em(LISA_CLR_H32, value_opnd, value_opnd);
    } else {
        lsassertm(bit_size == 64,
            "Do not support %d bits address reg.", bit_size);
    }
#endif
    ra_free_temp(offset_reg_opnd);
}


bool translate_shr(IR1_INST *pir1)
{
    EXTENSION_MODE em = ZERO_EXTENSION;
#ifndef TARGET_X86_64
    if (ir1_opnd_size(ir1_get_opnd(pir1, 0)) == 32) {
        em = SIGN_EXTENSION;
    }
#endif
    IR2_OPND src;
    IR2_OPND dest = ra_alloc_itemp();
    IR2_OPND count = ra_alloc_itemp();
    IR2_OPND label_exit = ir2_opnd_new_type(IR2_OPND_LABEL);
    int opnd0_size = ir1_opnd_size(ir1_get_opnd(pir1, 0));

    if (ir1_opnd_is_gpr(ir1_get_opnd(pir1, 0)) &&
        (opnd0_size == 32 || opnd0_size == 64)) {
        src = ra_alloc_gpr(ir1_opnd_base_reg_num(ir1_get_opnd(pir1, 0)));
        /*when reg is 32bit, we need clear high  32bit*/
        if (opnd0_size == 32) {
            la_append_ir2_opnd2_em(LISA_CLR_H32, src, src);
        }
    } else {
        src = load_ireg_from_ir1(ir1_get_opnd(pir1, 0), em, false);
    }
    /**
     *      shr  reg/mem{8/16/32/64}   imm8
     */
    if (ir1_opnd_is_imm(ir1_get_opnd(pir1, 1)) &&
        !ir1_need_calculate_any_flag(pir1)) {
        int src_opnd_1 = ir1_opnd_simm(ir1_get_opnd(pir1, 1));
        /*if imm is zero, directly return*/
#ifndef TARGET_X86_64
        if (!(src_opnd_1 & 0x1f)) {
            goto out;
        }
        la_append_ir2_opnd2i_em(LISA_SRLI_W, dest, src, src_opnd_1 & 0x1f);
#else
        if (!(src_opnd_1 & 0x3f)) {
            goto out;
        }
        la_append_ir2_opnd2i_em(LISA_SRLI_D, dest, src, src_opnd_1 & 0x3f);
#endif
        store_ireg_to_ir1(dest, ir1_get_opnd(pir1, 0), false);
    } else if (ir1_opnd_is_imm(ir1_get_opnd(pir1, 1)) &&
        ir1_need_calculate_any_flag(pir1)) {
        int src_opnd_1 = ir1_opnd_simm(ir1_get_opnd(pir1, 1));
        /*if imm is zero, directly return*/
#ifndef TARGET_X86_64
        if (!(src_opnd_1 & 0x1f)) {
            goto out;
        }
#else
        if (!(src_opnd_1 & 0x3f)) {
            goto out;
        }
#endif
        IR2_OPND  original_count = load_ireg_from_ir1(ir1_get_opnd(pir1, 1),
                                                UNKNOWN_EXTENSION, false);
        int32 mask = (ir1_opnd_size(ir1_get_opnd(pir1, 0)) == 64) ? 63 : 31;
        la_append_ir2_opnd2i_em(LISA_ANDI, count, original_count, mask);

#ifndef TARGET_X86_64
        la_append_ir2_opnd3_em(LISA_SRL_W, dest, src, count);
#else
        la_append_ir2_opnd3_em(LISA_SRL_D, dest, src, count);
#endif
        generate_eflag_calculation(dest, src, count, pir1, true);
        store_ireg_to_ir1(dest, ir1_get_opnd(pir1, 0), false);
    } else {
        IR2_OPND original_count =
                ra_alloc_gpr(ir1_opnd_base_reg_num(ir1_get_opnd(pir1, 1)));
        int32 mask = (ir1_opnd_size(ir1_get_opnd(pir1, 0)) == 64) ? 63 : 31;
        la_append_ir2_opnd2i_em(LISA_ANDI, count, original_count, mask);
        la_append_ir2_opnd3(LISA_BEQ, count, zero_ir2_opnd, label_exit);
#ifndef TARGET_X86_64
        la_append_ir2_opnd3_em(LISA_SRL_W, dest, src, count);
#else
        la_append_ir2_opnd3_em(LISA_SRL_D, dest, src, count);
#endif
        generate_eflag_calculation(dest, src, count, pir1, true);
        store_ireg_to_ir1(dest, ir1_get_opnd(pir1, 0), false);
        la_append_ir2_opnd1(LISA_LABEL, label_exit);
    }

out:
    ra_free_temp(count);
    ra_free_temp(dest);
    return true;
}
