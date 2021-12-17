bool translate_shr(IR1_INST *pir1)
{
    EXTENSION_MODE em = ZERO_EXTENSION;
#ifndef TARGET_X86_64
    if (ir1_opnd_size(ir1_get_opnd(pir1, 0)) == 32) {
        em = SIGN_EXTENSION;
    }
#endif
    IR2_OPND src = load_ireg_from_ir1(ir1_get_opnd(pir1, 0), em, false);
    IR2_OPND original_count =
        load_ireg_from_ir1(ir1_get_opnd(pir1, 0) + 1, ZERO_EXTENSION, false);
    IR2_OPND dest = ra_alloc_itemp();
    IR2_OPND count = ra_alloc_itemp();
#ifndef TARGET_X86_64
    la_append_ir2_opnd2i_em(LISA_ANDI, count, original_count, 31);
    IR2_OPND label_exit = ir2_opnd_new_type(IR2_OPND_LABEL);
    la_append_ir2_opnd3(LISA_BEQ, count, zero_ir2_opnd, label_exit);
    la_append_ir2_opnd3_em(LISA_SRL_W, dest, src, count);
#else
    int32 mask = (ir1_opnd_size(ir1_get_opnd(pir1, 0)) == 64) ? 63 : 31;
    la_append_ir2_opnd2i_em(LISA_ANDI, count, original_count, mask);
    IR2_OPND label_exit = ir2_opnd_new_type(IR2_OPND_LABEL);
    int opnd1_size = ir1_opnd_size(ir1_get_opnd(pir1, 0));
    if (opnd1_size == 32
        && ir1_opnd_is_gpr(ir1_get_opnd(pir1, 0))) {
        /* no d_slot anymore, so exec before branch */
        IR2_OPND gpr_opnd = ra_alloc_gpr(ir1_opnd_base_reg_num(ir1_get_opnd(pir1, 0)));
        la_append_ir2_opnd2_em(LISA_CLR_H32, gpr_opnd, gpr_opnd);
        la_append_ir2_opnd3(LISA_BEQ, count, zero_ir2_opnd, label_exit);
    } else {
        la_append_ir2_opnd3(LISA_BEQ, count, zero_ir2_opnd, label_exit);
    }

    la_append_ir2_opnd3_em(LISA_SRL_D, dest, src, count);
#endif
    generate_eflag_calculation(dest, src, count, pir1, true);
    store_ireg_to_ir1(dest, ir1_get_opnd(pir1, 0), false);
    la_append_ir2_opnd1(LISA_LABEL, label_exit);

    ra_free_temp(count);
    ra_free_temp(dest);
    return true;
}

bool translate_shr(IR1_INST *pir1)
{
    EXTENSION_MODE em = ZERO_EXTENSION;
#ifndef TARGET_X86_64
    if (ir1_opnd_size(ir1_get_opnd(pir1, 0)) == 32) {
        em = SIGN_EXTENSION;
    }
#endif

    IR2_OPND src ;

    IR2_OPND dest = ra_alloc_itemp();
    IR2_OPND count = ra_alloc_itemp();
    int opnd1_size = ir1_opnd_size(ir1_get_opnd(pir1, 0));

    IR2_OPND label_exit = ir2_opnd_new_type(IR2_OPND_LABEL);

    if (ir1_opnd_is_gpr(ir1_get_opnd(pir1, 0)) && (opnd1_size == 32 || opnd1_size == 64)) {
        /* no d_slot anymore, so exec before branch */
        src = ra_alloc_gpr(ir1_opnd_base_reg_num(ir1_get_opnd(pir1, 0)));
        if(opnd1_size == 32)
            la_append_ir2_opnd2_em(LISA_CLR_H32, src, src);
    } else {
        src = load_ireg_from_ir1(ir1_get_opnd(pir1, 0), em, false);
    }

    if (ir1_opnd_is_imm(ir1_get_opnd(pir1, 1)) && !ir1_need_calculate_any_flag(pir1)){
        int src_opnd_1 = ir1_opnd_simm(ir1_get_opnd(pir1, 1));
        if (!src_opnd_1){
            la_append_ir2_opnd1(LISA_B, label_exit);
        }
#ifndef TARGET_X86_64
        la_append_ir2_opnd2i_em(LISA_SRLI_W, dest, src,src_opnd_1 & 0x1f);
#else
        la_append_ir2_opnd2i_em(LISA_SRLI_D, dest, src,src_opnd_1 & 0x3f);
#endif
        store_ireg_to_ir1(dest, ir1_get_opnd(pir1, 0), false);
        if (!src_opnd_1){
            la_append_ir2_opnd1(LISA_LABEL, label_exit);
        }
    } else {
    IR2_OPND original_count;
    if (ir1_opnd_is_gpr(ir1_get_opnd(pir1, 1))){
            original_count = ra_alloc_gpr(ir1_opnd_base_reg_num(ir1_get_opnd(pir1, 1)));
    } else {
            original_count = load_ireg_from_ir1(ir1_get_opnd(pir1, 1), ZERO_EXTENSION, false);
    }
    int32 mask = (ir1_opnd_size(ir1_get_opnd(pir1, 0)) == 64) ? 63 : 31;
    la_append_ir2_opnd2i_em(LISA_ANDI, count, original_count, mask);
    //la_append_ir2_opnd3(LISA_BEQ, count, zero_ir2_opnd, label_exit);
#ifndef TARGET_X86_64
    la_append_ir2_opnd3_em(LISA_SRL_W, dest, src, count);
#else
    la_append_ir2_opnd3_em(LISA_SRL_D, dest, src, count);
#endif
    generate_eflag_calculation(dest, src, count, pir1, true);
    store_ireg_to_ir1(dest, ir1_get_opnd(pir1, 0), false);
   //la_append_ir2_opnd1(LISA_LABEL, label_exit);
    }
    ra_free_temp(count);
    ra_free_temp(dest);
    return true;
}

bool translate_shr(IR1_INST *pir1)
{
    int opnd0_size = ir1_opnd_size(ir1_get_opnd(pir1, 0));
    EXTENSION_MODE em = ZERO_EXTENSION;
#ifndef TARGET_X86_64
    if (opnd0_size == 32) {
        em = SIGN_EXTENSION;
    }
#endif

    IR2_OPND src_opnd_0, src_opnd_1, dest_opnd, count;
    IR2_OPND label_exit = ir2_opnd_new_type(IR2_OPND_LABEL);
    if (opnd0_size == 32 && ir1_opnd_is_gpr(ir1_get_opnd(pir1, 0))){
         src_opnd_0 = ra_alloc_gpr(ir1_opnd_base_reg_num(ir1_get_opnd(pir1, 0)));
        la_append_ir2_opnd2_em(LISA_CLR_H32, src_opnd_0, src_opnd_0);
    } else {
         src_opnd_0 = load_ireg_from_ir1(ir1_get_opnd(pir1, 0), em, false);
    }
    dest_opnd = ra_alloc_itemp();
    count = ra_alloc_itemp();

    if (ir1_opnd_is_imm(ir1_get_opnd(pir1, 0) + 1) && !ir1_need_calculate_any_flag(pir1)){
        int src_opnd_1 = ir1_opnd_simm(ir1_get_opnd(pir1, 0) + 1);
        if (!src_opnd_1){
            la_append_ir2_opnd1(LISA_B, label_exit);
        }
#ifndef TARGET_X86_64
        la_append_ir2_opnd2i_em(LISA_SRLI_W, dest_opnd, src_opnd_0,
                                src_opnd_1 & 0x1f);
#else
        la_append_ir2_opnd2i_em(LISA_SRLI_D, dest_opnd, src_opnd_0,
                                src_opnd_1 & 0x3f);
#endif
        store_ireg_to_ir1(dest_opnd, ir1_get_opnd(pir1, 0), false);
        if (!src_opnd_1){
            la_append_ir2_opnd1(LISA_LABEL, label_exit);
        }
    } else {
        if (ir1_opnd_is_gpr(ir1_get_opnd(pir1, 1))){
            src_opnd_1 = ra_alloc_gpr(ir1_opnd_base_reg_num(ir1_get_opnd(pir1, 1)));
        } else {
            src_opnd_1 = load_ireg_from_ir1(ir1_get_opnd(pir1, 1), ZERO_EXTENSION, false);
        }
        int32 mask = (ir1_opnd_size(ir1_get_opnd(pir1, 0)) == 64) ? 63 : 31;
        la_append_ir2_opnd2i_em(LISA_ANDI, count, src_opnd_1, mask);
        la_append_ir2_opnd3(LISA_BEQ, count, zero_ir2_opnd, label_exit);
#ifndef TARGET_X86_64
        la_append_ir2_opnd3_em(LISA_SRL_W, dest_opnd, src_opnd_1, count);
#else
        la_append_ir2_opnd3_em(LISA_SRL_D, dest_opnd, src_opnd_1, count);
#endif
         generate_eflag_calculation(dest_opnd, src_opnd_1, count, pir1, true);
         store_ireg_to_ir1(dest_opnd, ir1_get_opnd(pir1, 0), false);
         la_append_ir2_opnd1(LISA_LABEL, label_exit);
    }
    ra_free_temp(dest_opnd);
    ra_free_temp(count);
    return true;
}


bool translate_shr(IR1_INST *pir1)
{
    EXTENSION_MODE em = ZERO_EXTENSION;
#ifndef TARGET_X86_64
    if (ir1_opnd_size(ir1_get_opnd(pir1, 0)) == 32) {
        em = SIGN_EXTENSION;
    }
#endif
    int opnd0_size = ir1_opnd_size(ir1_get_opnd(pir1, 0));
    IR2_OPND src_opnd_0;
    if (ir1_opnd_is_gpr(ir1_get_opnd(pir1, 0))){
        src_opnd_0 = ra_alloc_gpr(ir1_opnd_base_reg_num(ir1_get_opnd(pir1, 0)));
        if (opnd0_size == 32)
            la_append_ir2_opnd2_em(LISA_CLR_H32, src_opnd_0, src_opnd_0);
    } else {
         src_opnd_0 = load_ireg_from_ir1(ir1_get_opnd(pir1, 0), em, false);
    }
    IR2_OPND src_opnd_1;
    IR2_OPND label_exit = ir2_opnd_new_type(IR2_OPND_LABEL);
    IR2_OPND dest_opnd = ra_alloc_itemp();
    IR2_OPND count = ra_alloc_itemp();

    if (ir1_opnd_is_imm(ir1_get_opnd(pir1, 0) + 1) && !ir1_need_calculate_any_flag(pir1)){
        int src_opnd_1 = ir1_opnd_simm(ir1_get_opnd(pir1, 0) + 1);
        if (!src_opnd_1){
            la_append_ir2_opnd1(LISA_B, label_exit);
        }
#ifndef TARGET_X86_64
        la_append_ir2_opnd2i_em(LISA_SRLI_W, dest_opnd, src_opnd_0,
                                src_opnd_1 & 0x1f);
#else
        la_append_ir2_opnd2i_em(LISA_SRLI_D, dest_opnd, src_opnd_0,
                                src_opnd_1 & 0x3f);
#endif
        store_ireg_to_ir1(dest_opnd, ir1_get_opnd(pir1, 0), false);
        if (!src_opnd_1){
            la_append_ir2_opnd1(LISA_LABEL, label_exit);
        }
    } else {
        if (ir1_opnd_is_gpr(ir1_get_opnd(pir1, 1))){
            src_opnd_1 = ra_alloc_gpr(ir1_opnd_base_reg_num(ir1_get_opnd(pir1, 1)));
        } else {
            src_opnd_1 = load_ireg_from_ir1(ir1_get_opnd(pir1, 1), ZERO_EXTENSION, false);
        }
        int32 mask = (ir1_opnd_size(ir1_get_opnd(pir1, 0)) == 64) ? 63 : 31;
        la_append_ir2_opnd2i_em(LISA_ANDI, count, src_opnd_1, mask);
        la_append_ir2_opnd3(LISA_BEQ, count, zero_ir2_opnd, label_exit);
#ifndef TARGET_X86_64
        la_append_ir2_opnd3_em(LISA_SRL_W, dest_opnd, src_opnd_1, count);
#else
        la_append_ir2_opnd3_em(LISA_SRL_D, dest_opnd, src_opnd_1, count);
#endif
         generate_eflag_calculation(dest_opnd, src_opnd_1, count, pir1, true);
         store_ireg_to_ir1(dest_opnd, ir1_get_opnd(pir1, 0), false);
         la_append_ir2_opnd1(LISA_LABEL, label_exit);
    }
    ra_free_temp(dest_opnd);
    ra_free_temp(count);
    return true;
}


bool translate_shr(IR1_INST *pir1)
{
    IR1_OPND_TYPE opnd0_type = ir1_opnd_type(ir1_get_opnd(pir1, 0));
    IR1_OPND_TYPE opnd1_type = ir1_opnd_type(ir1_get_opnd(pir1, 1));
    int32 opnd0_size = ir1_opnd_size(ir1_get_opnd(pir1, 0));
    EXTENSION_MODE em = ZERO_EXTENSION;
    int32 mask ;
#ifndef TARGET_X86_64
    if (opnd0_size == 32) {
        em = SIGN_EXTENSION;
    }
#endif

    IR2_OPND src_opnd_0, src_opnd_1, dest_opnd, count;
    switch (opnd0_type)
    {
    case X86_OP_REG:
        switch (opnd1_type)
        {
        case X86_OP_REG:
            if (opnd0_size == 64 ){
                src_opnd_0 = ra_alloc_gpr(ir1_opnd_base_reg_num(ir1_get_opnd(pir1, 0)));

                src_opnd_1 = ra_alloc_gpr(ir1_opnd_base_reg_num(ir1_get_opnd(pir1, 1)));
                dest_opnd = ra_alloc_itemp();
                count = ra_alloc_itemp();
                mask = (ir1_opnd_size(ir1_get_opnd(pir1, 0)) == 64) ? 63 : 31;
                la_append_ir2_opnd2i_em(LISA_ANDI, count, src_opnd_1, mask);
#ifndef TARGET_x86_64
                la_append_ir2_opnd3_em(LISA_SRL_W, dest_opnd, src_opnd_0, count);
#else
                la_append_ir2_opnd3_em(LISA_SRL_D, dest_opnd, src_opnd_0, count);
#endif
                generate_eflag_calculation(dest_opnd, src_opnd_0, src_opnd_1, pir1, true);
                store_ireg_to_ir1(dest_opnd, ir1_get_opnd(pir1, 0), false);
                ra_free_temp(dest_opnd);
                ra_free_temp(count);

            } else {
                goto _xx_reg8;
            }
            break;
        case X86_OP_IMM:
            if (opnd0_size == 32 && ir1_get_opnd(pir1, 1)->imm < 0x1f){
                src_opnd_0 = ra_alloc_gpr(ir1_opnd_base_reg_num(ir1_get_opnd(pir1, 0)));
                 la_append_ir2_opnd2_em(LISA_MOV32_ZX, src_opnd_0, src_opnd_0);
                if (ir1_need_calculate_any_flag(pir1)) {
                    src_opnd_1 = load_ireg_from_ir1(ir1_get_opnd(pir1, 1),em, false);
                    generate_eflag_calculation(src_opnd_0, src_opnd_0,src_opnd_1,pir1, true);
                    la_append_ir2_opnd3_em(LISA_SRL_W, src_opnd_0, src_opnd_0, src_opnd_1);
                } else {
                    int src_opnd_1 = (int)ir1_opnd_uimm(ir1_get_opnd(pir1, 1));
                    la_append_ir2_opnd2i_em(LISA_SRLI_W,src_opnd_0,src_opnd_0,src_opnd_1 & 0x1f);
                }
            } else if (opnd0_size == 64 && ir1_get_opnd(pir1, 1)->imm < 0x3f){
                src_opnd_0 = ra_alloc_gpr(ir1_opnd_base_reg_num(ir1_get_opnd(pir1, 0)));
                if (ir1_need_calculate_any_flag(pir1)) {
                    src_opnd_1 = load_ireg_from_ir1(ir1_get_opnd(pir1, 1),em, false);
                    generate_eflag_calculation(src_opnd_0, src_opnd_0,src_opnd_1,pir1, true);
                    la_append_ir2_opnd3_em(LISA_SRL_D, src_opnd_0, src_opnd_0, src_opnd_1);
                } else {
                    int src_opnd_1 = (int)ir1_opnd_simm(ir1_get_opnd(pir1, 1));
                    la_append_ir2_opnd2i_em(LISA_SRLI_D,src_opnd_0,src_opnd_0,src_opnd_1 & 0x3f);
                }

            } else if (ir1_get_opnd(pir1, 1)->imm < 0x1f){
                src_opnd_0 = load_ireg_from_ir1(ir1_get_opnd(pir1, 0), em, false);
                dest_opnd = ra_alloc_itemp();
                if (ir1_need_calculate_any_flag(pir1)) {
                    src_opnd_1 = load_ireg_from_ir1(ir1_get_opnd(pir1, 1),em, false);
                    generate_eflag_calculation(dest_opnd, src_opnd_0,src_opnd_1,pir1, true);
                    la_append_ir2_opnd3_em(LISA_SRL_W, dest_opnd, src_opnd_0, src_opnd_1);
                } else {
                    int src_opnd_1 = (int)ir1_opnd_uimm(ir1_get_opnd(pir1, 1));
                    la_append_ir2_opnd2i_em(LISA_SRLI_W,dest_opnd,src_opnd_0,src_opnd_1 & 0x1f);
                }
                store_ireg_to_ir1(dest_opnd, ir1_get_opnd(pir1, 0), false);
                ra_free_temp(dest_opnd);
            } else {
                goto _default;
            }
            break;
        default:
             goto _default;
            break;
        }
        break;
    case X86_OP_MEM:
        switch (opnd1_type)
        {
        case X86_OP_REG:
            goto _xx_reg8;
            break;
        case X86_OP_IMM:
            if (opnd0_size <= 32 && ir1_get_opnd(pir1,1)->imm < 0x1f){
                src_opnd_0 = load_ireg_from_ir1(ir1_get_opnd(pir1, 0), em, false);

                dest_opnd = ra_alloc_itemp();
                if (ir1_need_calculate_any_flag(pir1)) {
                    src_opnd_1 = load_ireg_from_ir1(ir1_get_opnd(pir1, 1),em, false);
                    generate_eflag_calculation(dest_opnd, src_opnd_0,src_opnd_1,pir1, true);
                    la_append_ir2_opnd3_em(LISA_SRL_W, dest_opnd, src_opnd_0, src_opnd_1);
                } else {
                    int src_opnd_1 = (int)ir1_opnd_uimm(ir1_get_opnd(pir1, 1));
                    la_append_ir2_opnd2i_em(LISA_SRLI_W,dest_opnd,src_opnd_0,src_opnd_1 & 0x1f);
                }
                store_ireg_to_ir1(dest_opnd, ir1_get_opnd(pir1, 0), false);
                ra_free_temp(dest_opnd);
            } else if (opnd0_size == 64 && ir1_get_opnd(pir1,1)->imm < 0x3f){
                src_opnd_0 = load_ireg_from_ir1(ir1_get_opnd(pir1, 0), em, false);
                dest_opnd = ra_alloc_itemp();
                if (ir1_need_calculate_any_flag(pir1)) {
                    src_opnd_1 = load_ireg_from_ir1(ir1_get_opnd(pir1, 1),em, false);
                    generate_eflag_calculation(dest_opnd, src_opnd_0,src_opnd_1,pir1, true);
                    la_append_ir2_opnd3_em(LISA_SRL_D, dest_opnd, src_opnd_0, src_opnd_1);
                } else {
                    int src_opnd_1 = (int)ir1_opnd_uimm(ir1_get_opnd(pir1, 1));
                    la_append_ir2_opnd2i_em(LISA_SRLI_D,dest_opnd,src_opnd_0,src_opnd_1 & 0x3f);
                }
                store_ireg_to_ir1(dest_opnd, ir1_get_opnd(pir1, 0), false);
                ra_free_temp(dest_opnd);
            } else {
                goto _default;
            }
            break;
        default:
             goto _default;
            break;
        }
        break;
    default:
         goto _default;
        break;
    }
    return true;

_xx_reg8:
    src_opnd_0 = load_ireg_from_ir1(ir1_get_opnd(pir1, 0), em, false);

    src_opnd_1 = ra_alloc_gpr(ir1_opnd_base_reg_num(ir1_get_opnd(pir1, 1)));
    dest_opnd = ra_alloc_itemp();
    count = ra_alloc_itemp();

    la_append_ir2_opnd2i_em(LISA_ANDI, count, src_opnd_1,31);
#ifndef TARGET_x86_64
            la_append_ir2_opnd3_em(LISA_SRL_W, dest_opnd, src_opnd_0, count);
#else
            la_append_ir2_opnd3_em(LISA_SRL_D, dest_opnd, src_opnd_0, count);
#endif
    generate_eflag_calculation(dest_opnd, src_opnd_0, count, pir1, true);
    store_ireg_to_ir1(dest_opnd, ir1_get_opnd(pir1, 0), false);
    ra_free_temp(dest_opnd);
    ra_free_temp(count);
    return true;

_default:
    src_opnd_0 = load_ireg_from_ir1(ir1_get_opnd(pir1, 0), SIGN_EXTENSION, false);
    src_opnd_1 =
        load_ireg_from_ir1(ir1_get_opnd(pir1, 0) + 1, ZERO_EXTENSION, false);
    dest_opnd = ra_alloc_itemp();
    count = ra_alloc_itemp();
    mask = (ir1_opnd_size(ir1_get_opnd(pir1, 0)) == 64) ? 63 : 31;
    la_append_ir2_opnd2i_em(LISA_ANDI, count, src_opnd_1, mask);
#ifndef TARGET_x86_64
            la_append_ir2_opnd3_em(LISA_SRL_W, dest_opnd, src_opnd_0, count);
#else
            la_append_ir2_opnd3_em(LISA_SRL_D, dest_opnd, src_opnd_0, count);
#endif
    generate_eflag_calculation(dest_opnd, src_opnd_0, count, pir1, true);
    store_ireg_to_ir1(dest_opnd, ir1_get_opnd(pir1, 0), false);
    ra_free_temp(dest_opnd);
    ra_free_temp(count);
    return true;
}

