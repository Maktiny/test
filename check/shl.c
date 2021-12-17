int main(){
    /*at&t: opcode source dest*/
    asm(
        "shl $0, %bh \n\t"
        "shl $1, %ax \n\t"
        "shl $1, %ebx \n\t"
        "shl $1, %rax \n\t"

        "shl %cl, %bh \n\t"
        "shl %cl, %dx \n\t"
        "shl %cl, %ebx \n\t"
        "shl %cl, %rax \n\t"

        "shl $0xff, %rax \n\t"
        "shl $0x11, %rax \n\t"

        "shl $0xff, %bh \n\t"
        "shl $0xff, %bx \n\t"
        "shl $0x01, %ebx \n\t"
        "shl $0x3f, %rbx \n\t"

    );
    return 0;
}

// bool translate_shl(IR1_INST *pir1)
// {
//     IR2_OPND src ;
//     IR2_OPND dest = ra_alloc_itemp();
//     IR2_OPND count = ra_alloc_itemp();
//     int opnd1_size = ir1_opnd_size(ir1_get_opnd(pir1, 0));
//     IR2_OPND label_exit = ir2_opnd_new_type(IR2_OPND_LABEL);

//     if (ir1_opnd_is_gpr(ir1_get_opnd(pir1, 0)) &&
//         (opnd1_size == 32 || opnd1_size == 64)) {
//         src = ra_alloc_gpr(ir1_opnd_base_reg_num(ir1_get_opnd(pir1, 0)));
//         /*when reg is 32bit, we need clear high  32bit*/
//         if ( opnd1_size == 32) {
//             la_append_ir2_opnd2_em(LISA_MOV32_ZX, src, src);
//         }
//     } else {
//         src = load_ireg_from_ir1(ir1_get_opnd(pir1, 0), SIGN_EXTENSION, false);
//     }
//     /**
//      *      shl  reg/mem{8/16/32/64}   imm8
//      */
//     if (ir1_opnd_is_imm(ir1_get_opnd(pir1, 1)) &&
//         !ir1_need_calculate_any_flag(pir1)) {
//         int src_opnd_1 = ir1_opnd_simm(ir1_get_opnd(pir1, 1));
//         /*if imm is zero, jump to next instruction*/
//         if (!src_opnd_1) {
//             la_append_ir2_opnd1(LISA_B, label_exit);
//         }
// #ifndef TARGET_X86_64
//         la_append_ir2_opnd2i_em(LISA_SLLI_W, dest, src, src_opnd_1 & 0x1f);
// #else
//         la_append_ir2_opnd2i_em(LISA_SLLI_D, dest, src, src_opnd_1 & 0x3f);
// #endif
//         store_ireg_to_ir1(dest, ir1_get_opnd(pir1, 0), false);
//         if (!src_opnd_1) {
//             la_append_ir2_opnd1(LISA_LABEL, label_exit);
//         }
//     } else {
//         IR2_OPND original_count;
//         if (ir1_opnd_is_gpr(ir1_get_opnd(pir1, 1))) {
//             original_count =
//                 ra_alloc_gpr(ir1_opnd_base_reg_num(ir1_get_opnd(pir1, 1)));
//         } else {
//             original_count = load_ireg_from_ir1(ir1_get_opnd(pir1, 1),
//                                                 ZERO_EXTENSION, false);
//         }
//         int32 mask = (ir1_opnd_size(ir1_get_opnd(pir1, 0)) == 64) ? 63 : 31;
//         la_append_ir2_opnd2i_em(LISA_ANDI, count, original_count, mask);
//         la_append_ir2_opnd3(LISA_BEQ, count, zero_ir2_opnd, label_exit);
// #ifndef TARGET_X86_64
//         la_append_ir2_opnd3_em(LISA_SLL_W, dest, src, count);
// #else
//         la_append_ir2_opnd3_em(LISA_SLL_D, dest, src, count);
// #endif
//         generate_eflag_calculation(dest, src, count, pir1, true);
//         store_ireg_to_ir1(dest, ir1_get_opnd(pir1, 0), false);
//         la_append_ir2_opnd1(LISA_LABEL, label_exit);
//     }
//     ra_free_temp(count);
//     ra_free_temp(dest);
//     return true;
// }



// bool translate_shl_new(IR1_INST *pir1)
// {
//     IR2_OPND src ;
//     IR2_OPND dest = ra_alloc_itemp();
//     IR2_OPND count = ra_alloc_itemp();
//     int opnd1_size = ir1_opnd_size(ir1_get_opnd(pir1, 0));
//     IR2_OPND label_exit = ir2_opnd_new_type(IR2_OPND_LABEL);

//     if (ir1_opnd_is_gpr(ir1_get_opnd(pir1, 0)) &&
//         (opnd1_size == 32 || opnd1_size == 64)) {
//         src = ra_alloc_gpr(ir1_opnd_base_reg_num(ir1_get_opnd(pir1, 0)));
//         /*when reg is 32bit, we need clear high  32bit*/
//         if ( opnd1_size == 32) {
//             la_append_ir2_opnd2_em(LISA_MOV32_ZX, src, src);
//         }
//     } else {
//         src = load_ireg_from_ir1(ir1_get_opnd(pir1, 0), SIGN_EXTENSION, false);
//     }
//     /**
//      *      shl  reg/mem{8/16/32/64}   imm8
//      */
//     if (ir1_opnd_is_imm(ir1_get_opnd(pir1, 1)) &&
//         !ir1_need_calculate_any_flag(pir1)) {
//         int src_opnd_1 = ir1_opnd_simm(ir1_get_opnd(pir1, 1));
//         /*if imm is zero, jump to next instruction*/
//         if (!src_opnd_1) {
//             la_append_ir2_opnd1(LISA_B, label_exit);
//         }
// #ifndef TARGET_X86_64
//         la_append_ir2_opnd2i_em(LISA_SLLI_W, dest, src, src_opnd_1 & 0x1f);
// #else
//         la_append_ir2_opnd2i_em(LISA_SLLI_D, dest, src, src_opnd_1 & 0x3f);
// #endif
//         store_ireg_to_ir1(dest, ir1_get_opnd(pir1, 0), false);
//         if (!src_opnd_1) {
//             la_append_ir2_opnd1(LISA_LABEL, label_exit);
//         }
//     } else {
//         IR2_OPND original_count;
//         if (ir1_opnd_is_gpr(ir1_get_opnd(pir1, 1))) {
//             original_count =
//                 ra_alloc_gpr(ir1_opnd_base_reg_num(ir1_get_opnd(pir1, 1)));
//         } else {
//             original_count = load_ireg_from_ir1(ir1_get_opnd(pir1, 1),
//                                                 ZERO_EXTENSION, false);
//         }
//         int32 mask = (ir1_opnd_size(ir1_get_opnd(pir1, 0)) == 64) ? 63 : 31;
//         la_append_ir2_opnd2i_em(LISA_ANDI, count, original_count, mask);
//         la_append_ir2_opnd3(LISA_BEQ, count, zero_ir2_opnd, label_exit);
// #ifndef TARGET_X86_64
//         la_append_ir2_opnd3_em(LISA_SLL_W, dest, src, count);
// #else
//         la_append_ir2_opnd3_em(LISA_SLL_D, dest, src, count);
// #endif
//         generate_eflag_calculation(dest, src, count, pir1, true);
//         store_ireg_to_ir1(dest, ir1_get_opnd(pir1, 0), false);
//         la_append_ir2_opnd1(LISA_LABEL, label_exit);
//     }
//     ra_free_temp(count);
//     ra_free_temp(dest);
//     return true;
// }
