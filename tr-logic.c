#include "common.h"
#include "reg-alloc.h"
#include "flag-lbt.h"
#include "latx-options.h"
#include "translate.h"

static bool translate_shrd_imm(IR1_INST *pir1);
static bool translate_shrd_cl(IR1_INST *pir1);
static bool translate_shld_cl(IR1_INST *pir1);
static bool translate_shld_imm(IR1_INST *pir1);

void la_append_8bitReg_ir2_opnd2(IR1_INST *pir1, IR2_OPCODE opcode);
bool deal_8h_ir2_opnd2(IR1_INST *pir1, IR2_OPCODE universal_opcode,
                       IR2_OPCODE imm_opcode, bool deal_imm12);
bool excluded_8h_ir2_opnd2(IR1_INST *pir1, IR2_OPCODE opcode,
                           IR2_OPCODE opcode_imm12, bool imm12);

bool universial(IR1_INST *pir1, IR2_OPCODE opcode);
bool mem_imm(IR1_INST *pir1, IR2_OPCODE opcode, IR2_OPCODE opcode_imm12,
             bool deal_imm12);
bool mem_reg(IR1_INST *pir1, IR2_OPCODE opcode);
bool reg_reg(IR1_INST *pir1, IR2_OPCODE opcode);
bool reg_imm(IR1_INST *pir1, IR2_OPCODE opcode, IR2_OPCODE opcode_imm12,
             bool deal_imm12);
bool reg_mem(IR1_INST *pir1, IR2_OPCODE opcode);

bool reg_reg(IR1_INST *pir1, IR2_OPCODE opcode)
{
    if (unlikely(ir1_opnd_is_8h(ir1_get_opnd(pir1, 0)) ||
                 ir1_opnd_is_8h(ir1_get_opnd(pir1, 1)))) {
        la_append_8bitReg_ir2_opnd2(pir1, opcode);
    } else {
        IR2_OPND src_opnd_0 =
            ra_alloc_gpr(ir1_opnd_base_reg_num(ir1_get_opnd(pir1, 0)));
        IR2_OPND src_opnd_1 =
            ra_alloc_gpr(ir1_opnd_base_reg_num(ir1_get_opnd(pir1, 1)));
        generate_eflag_calculation(src_opnd_0, src_opnd_0, src_opnd_1, pir1,
                                   true);
        if (ir1_opnd_size(ir1_get_opnd(pir1, 0)) == 64)
            la_append_ir2_opnd3(opcode, src_opnd_0, src_opnd_0, src_opnd_1);
        else {
            IR2_OPND tmp = ra_alloc_itemp();
            la_append_ir2_opnd3(opcode, tmp, src_opnd_0, src_opnd_1);
            store_ireg_to_ir1(tmp, ir1_get_opnd(pir1, 0), false);
            ra_free_temp(tmp);
        }
    }
    return true;
}

bool reg_imm(IR1_INST *pir1, IR2_OPCODE opcode, IR2_OPCODE opcode_imm12,
             bool deal_imm12)
{
    IR2_OPND src_opnd_0;
    if (unlikely(ir1_opnd_is_8h(ir1_get_opnd(pir1, 0)))) {
        IR2_OPND dest_opnd = ra_alloc_itemp();
        src_opnd_0 =
            load_ireg_from_ir1(ir1_get_opnd(pir1, 0), SIGN_EXTENSION, false);
        // if (deal_imm12 &&
        //     ir1_opnd_is_uimm_within_12bit(ir1_get_opnd(pir1, 1)) &&
        //     !ir1_need_calculate_any_flag(pir1)) {
        //     la_append_ir2_opnd2i(opcode_imm12, dest_opnd, src_opnd_0,
        //                          ir1_opnd_uimm(ir1_get_opnd(pir1, 1)));
        // } else
        {
            IR2_OPND src_opnd_1 = load_ireg_from_ir1(ir1_get_opnd(pir1, 1),
                                                     SIGN_EXTENSION, false);
            la_append_ir2_opnd3_em(opcode, dest_opnd, src_opnd_0, src_opnd_1);
            generate_eflag_calculation(dest_opnd, src_opnd_0, src_opnd_1, pir1,
                                       true);
        }
        store_ireg_to_ir1(dest_opnd, ir1_get_opnd(pir1, 0), false);
        ra_free_temp(dest_opnd);
    } else if (ir1_opnd_is_uimm_within_12bit(ir1_get_opnd(pir1, 1)) &&
               deal_imm12 && !ir1_need_calculate_any_flag(pir1)) {
        src_opnd_0 = ra_alloc_gpr(ir1_opnd_base_reg_num(ir1_get_opnd(pir1, 0)));
        int opnd0_size = ir1_opnd_size(ir1_get_opnd(pir1, 0));
        if (opnd0_size == 64)
            la_append_ir2_opnd2i(opcode_imm12, src_opnd_0, src_opnd_0,
                                 ir1_get_opnd(pir1, 1)->imm);
        else {
            IR2_OPND tmp = ra_alloc_itemp();
            la_append_ir2_opnd2i(opcode_imm12, tmp, src_opnd_0,
                                 ir1_get_opnd(pir1, 1)->imm);
            if (opnd0_size == 32)
                la_append_ir2_opnd2ii(LISA_BSTRPICK_D, src_opnd_0, tmp, 31, 0);
            else
                la_append_ir2_opnd2ii(LISA_BSTRINS_D, src_opnd_0, tmp,
                                      opnd0_size - 1, 0);
            ra_free_temp(tmp);
        }
    } else {
        src_opnd_0 = ra_alloc_gpr(ir1_opnd_base_reg_num(ir1_get_opnd(pir1, 0)));
        IR2_OPND src_opnd_1 =
            load_ireg_from_ir1(ir1_get_opnd(pir1, 1), SIGN_EXTENSION, false);
        if (ir1_opnd_size(ir1_get_opnd(pir1, 0)) == 64) {
            generate_eflag_calculation(src_opnd_0, src_opnd_0, src_opnd_1, pir1,
                                       true);
            la_append_ir2_opnd3_em(opcode, src_opnd_0, src_opnd_0, src_opnd_1);
        } else {
            IR2_OPND dest_opnd = ra_alloc_itemp();
            la_append_ir2_opnd3_em(opcode, dest_opnd, src_opnd_0, src_opnd_1);
            generate_eflag_calculation(dest_opnd, src_opnd_0, src_opnd_1, pir1,
                                       true);
            store_ireg_to_ir1(dest_opnd, ir1_get_opnd(pir1, 0), false);
            ra_free_temp(dest_opnd);
        }
    }

//         IR2_OPND src_opnd_1 =
//         load_ireg_from_ir1(ir1_get_opnd(pir1, 1), SIGN_EXTENSION, false);
//     src_opnd_0 =
//         load_ireg_from_ir1(ir1_get_opnd(pir1, 0), SIGN_EXTENSION, false);
// #ifndef TARGET_X86_64
//     IR2_OPND dest_opnd = ra_alloc_itemp();
//     la_append_ir2_opnd3_em(opcode, dest_opnd, src_opnd_0, src_opnd_1);
//     generate_eflag_calculation(dest_opnd, src_opnd_0, src_opnd_1, pir1, true);
//     store_ireg_to_ir1(dest_opnd, ir1_get_opnd(pir1, 0), false);
//     ra_free_temp(dest_opnd);
// #else
//     if (ir1_opnd_size(ir1_get_opnd(pir1, 0)) == 64
//         && ir1_opnd_is_gpr(ir1_get_opnd(pir1, 0))) {
//         int reg_num = ir1_opnd_base_reg_num(ir1_get_opnd(pir1, 0));
//         IR2_OPND dest_opnd = ra_alloc_gpr(reg_num);
//         la_append_ir2_opnd3_em(opcode, dest_opnd, src_opnd_0, src_opnd_1);
//         generate_eflag_calculation(dest_opnd, src_opnd_0, src_opnd_1, pir1, true);
//     } else {
//         IR2_OPND dest_opnd = ra_alloc_itemp();
//         la_append_ir2_opnd3_em(opcode, dest_opnd, src_opnd_0, src_opnd_1);
//         generate_eflag_calculation(dest_opnd, src_opnd_0, src_opnd_1, pir1, true);
//         store_ireg_to_ir1(dest_opnd, ir1_get_opnd(pir1, 0), false);
//         ra_free_temp(dest_opnd);
//     }
// #endif

    return true;
}
bool reg_mem(IR1_INST *pir1, IR2_OPCODE opcode)
{
    IR2_OPND src_opnd_0;
    if (unlikely(ir1_opnd_is_8h(ir1_get_opnd(pir1, 0)))) {
        src_opnd_0 =
            load_ireg_from_ir1(ir1_get_opnd(pir1, 0), SIGN_EXTENSION, false);
    } else {
        src_opnd_0 = ra_alloc_gpr(ir1_opnd_base_reg_num(ir1_get_opnd(pir1, 0)));
    }
    IR2_OPND dest_opnd = ra_alloc_itemp();
    IR2_OPND src_opnd_1 =
        load_ireg_from_ir1(ir1_get_opnd(pir1, 1), SIGN_EXTENSION, false);
    la_append_ir2_opnd3_em(opcode, dest_opnd, src_opnd_0, src_opnd_1);
    generate_eflag_calculation(dest_opnd, src_opnd_0, src_opnd_1, pir1, true);
    store_ireg_to_ir1(dest_opnd, ir1_get_opnd(pir1, 0), false);
    ra_free_temp(dest_opnd);
    return true;
}
bool mem_reg(IR1_INST *pir1, IR2_OPCODE opcode)
{
    IR2_OPND src_opnd_1;
    if (unlikely(ir1_opnd_is_8h(ir1_get_opnd(pir1, 1)))) {
        src_opnd_1 =
            load_ireg_from_ir1(ir1_get_opnd(pir1, 1), SIGN_EXTENSION, false);
    } else {
        src_opnd_1 = ra_alloc_gpr(ir1_opnd_base_reg_num(ir1_get_opnd(pir1, 1)));
    }
    IR2_OPND dest_opnd = ra_alloc_itemp();
    IR2_OPND src_opnd_0 =
        load_ireg_from_ir1(ir1_get_opnd(pir1, 0), SIGN_EXTENSION, false);
    la_append_ir2_opnd3_em(opcode, dest_opnd, src_opnd_0, src_opnd_1);
    generate_eflag_calculation(dest_opnd, src_opnd_0, src_opnd_1, pir1, true);
    store_ireg_to_ir1(dest_opnd, ir1_get_opnd(pir1, 0), false);
    ra_free_temp(dest_opnd);
    return true;
}
bool mem_imm(IR1_INST *pir1, IR2_OPCODE opcode, IR2_OPCODE opcode_imm12,
             bool deal_imm12)
{
    IR2_OPND src_opnd_0 =
        load_ireg_from_ir1(ir1_get_opnd(pir1, 0), SIGN_EXTENSION, false);
    if (deal_imm12 && ir1_opnd_is_uimm_within_12bit(ir1_get_opnd(pir1, 1)) &&
        !ir1_need_calculate_any_flag(pir1)) {
        int opnd0_size = ir1_opnd_size(ir1_get_opnd(pir1, 0));
        IR2_OPND tmp = ra_alloc_itemp();
        la_append_ir2_opnd2i(opcode_imm12, tmp, src_opnd_0,
                             ir1_get_opnd(pir1, 1)->imm);
        if (opnd0_size == 32)
            la_append_ir2_opnd2ii(LISA_BSTRPICK_D, src_opnd_0, tmp, 31, 0);
        else
            la_append_ir2_opnd2ii(LISA_BSTRINS_D, src_opnd_0, tmp,
                                  opnd0_size - 1, 0);
        ra_free_temp(tmp);
    } else {
        IR2_OPND dest_opnd = ra_alloc_itemp();
        IR2_OPND src_opnd_1 =
            load_ireg_from_ir1(ir1_get_opnd(pir1, 1), SIGN_EXTENSION, false);
        la_append_ir2_opnd3_em(opcode, dest_opnd, src_opnd_0, src_opnd_1);
        generate_eflag_calculation(dest_opnd, src_opnd_0, src_opnd_1, pir1,
                                   true);
        store_ireg_to_ir1(dest_opnd, ir1_get_opnd(pir1, 0), false);
        ra_free_temp(dest_opnd);
    }
    return true;
}

bool universial(IR1_INST *pir1, IR2_OPCODE opcode)
{
    IR2_OPND src_opnd_1 =
        load_ireg_from_ir1(ir1_get_opnd(pir1, 1), SIGN_EXTENSION, false);
    IR2_OPND src_opnd_0 =
        load_ireg_from_ir1(ir1_get_opnd(pir1, 0), SIGN_EXTENSION, false);
#ifndef TARGET_X86_64
    IR2_OPND dest_opnd = ra_alloc_itemp();
    la_append_ir2_opnd3_em(opcode, dest_opnd, src_opnd_0, src_opnd_1);
    generate_eflag_calculation(dest_opnd, src_opnd_0, src_opnd_1, pir1, true);
    store_ireg_to_ir1(dest_opnd, ir1_get_opnd(pir1, 0), false);
    ra_free_temp(dest_opnd);
#else
    if (ir1_opnd_size(ir1_get_opnd(pir1, 0)) == 64
        && ir1_opnd_is_gpr(ir1_get_opnd(pir1, 0))) {
        /* if the target reg size is 64 bits */
        /* we can save it directly */
        int reg_num = ir1_opnd_base_reg_num(ir1_get_opnd(pir1, 0));
        IR2_OPND dest_opnd = ra_alloc_gpr(reg_num);
        generate_eflag_calculation(dest_opnd, src_opnd_0, src_opnd_1, pir1, true);
        la_append_ir2_opnd3_em(opcode, dest_opnd, src_opnd_0, src_opnd_1);
    } else {
        IR2_OPND dest_opnd = ra_alloc_itemp();
        la_append_ir2_opnd3_em(opcode, dest_opnd, src_opnd_0, src_opnd_1);
        generate_eflag_calculation(dest_opnd, src_opnd_0, src_opnd_1, pir1, true);
        store_ireg_to_ir1(dest_opnd, ir1_get_opnd(pir1, 0), false);
        ra_free_temp(dest_opnd);
    }
#endif
    return true;
}

void la_append_8bitReg_ir2_opnd2(IR1_INST *pir1, IR2_OPCODE opcode)
{
    int reg_num0 = ir1_opnd_base_reg_num(ir1_get_opnd(pir1, 0));
    int reg_num1 = ir1_opnd_base_reg_num(ir1_get_opnd(pir1, 1));
    IR2_OPND src_opnd_0 = ra_alloc_gpr(reg_num0);
    IR2_OPND src_opnd_1 = ra_alloc_gpr(reg_num1);
    IR2_OPND tmp0 = ra_alloc_itemp();
    IR2_OPND tmp1 = ra_alloc_itemp();
    if (ir1_opnd_is_8h(ir1_get_opnd(pir1, 0))) {
        if (ir1_opnd_is_8l(ir1_get_opnd(pir1, 1))) {
            la_append_ir2_opnd2i(LISA_SRLI_D, tmp0, src_opnd_0, 8);
            generate_eflag_calculation(tmp0, tmp0, src_opnd_1, pir1, true);
            la_append_ir2_opnd3(opcode, tmp0, tmp0, src_opnd_1);
        } else {
            if (ir1_need_calculate_any_flag(pir1)) {
                la_append_ir2_opnd2i(LISA_SRLI_D, tmp0, src_opnd_0, 8);
                la_append_ir2_opnd2i(LISA_SRLI_D, tmp1, src_opnd_1, 8);
                generate_eflag_calculation(tmp0, tmp0, tmp1, pir1, true);
            }
            la_append_ir2_opnd3(opcode, tmp0, src_opnd_0, src_opnd_1);
        }
        la_append_ir2_opnd2ii(LISA_BSTRINS_D, src_opnd_0, tmp0, 15, 8);
    } else if (ir1_opnd_is_8l(ir1_get_opnd(pir1, 0))) {
        if (ir1_opnd_is_8h(ir1_get_opnd(pir1, 1))) {
            la_append_ir2_opnd2i(LISA_SRLI_D, tmp1, src_opnd_1, 8);
            generate_eflag_calculation(src_opnd_0, src_opnd_0, tmp1, pir1,
                                       true);
            la_append_ir2_opnd3(opcode, tmp0, src_opnd_0, tmp1);
        } else {
            la_append_ir2_opnd3(opcode, tmp0, src_opnd_0, src_opnd_1);
            generate_eflag_calculation(src_opnd_0, src_opnd_0, src_opnd_1, pir1,
                                       true);
        }
        la_append_ir2_opnd2ii(LISA_BSTRINS_D, src_opnd_0, tmp0, 7, 0);
    } else {
        lsassertm(0, "la_append_8bitReg_ir2_opnd2(): pir1 is not 8 bit \n");
    }
    ra_free_temp(tmp0);
    ra_free_temp(tmp1);
}

// bool deal_8h_ir2_opnd2(IR1_INST *pir1, IR2_OPCODE universal_opcode,
//                        IR2_OPCODE imm_opcode, bool deal_imm12)
// {
//     if (ir1_opnd_is_gpr(ir1_get_opnd(pir1, 0)) &&
//         ir1_opnd_is_gpr(ir1_get_opnd(pir1, 1))) {
//         la_append_8bitReg_ir2_opnd2(pir1, universal_opcode);
//         return true;
//     }
//     IR2_OPND dest_opnd = ra_alloc_itemp();
//     IR2_OPND src_opnd_0 =
//         load_ireg_from_ir1(ir1_get_opnd(pir1, 0), SIGN_EXTENSION, false);
//     if (deal_imm12 && ir1_opnd_is_uimm_within_12bit(ir1_get_opnd(pir1, 1)) &&
//         !ir1_need_calculate_any_flag(pir1)) {
//         la_append_ir2_opnd2i(imm_opcode, dest_opnd, src_opnd_0,
//                              ir1_opnd_uimm(ir1_get_opnd(pir1, 1)));
//     } else {
//         IR2_OPND src_opnd_1 =
//             load_ireg_from_ir1(ir1_get_opnd(pir1, 1), SIGN_EXTENSION, false);
//         la_append_ir2_opnd3_em(universal_opcode, dest_opnd, src_opnd_0,
//                                src_opnd_1);
//         generate_eflag_calculation(dest_opnd, src_opnd_0, src_opnd_1, pir1,
//                                    true);
//     }
//     store_ireg_to_ir1(dest_opnd, ir1_get_opnd(pir1, 0), false);
//     ra_free_temp(dest_opnd);
//     return true;
// }

// bool excluded_8h_ir2_opnd2(IR1_INST *pir1, IR2_OPCODE opcode,
//                            IR2_OPCODE opcode_imm12, bool imm12)
// {
//     IR2_OPND src_opnd_0, src_opnd_1;
//     IR2_OPND tmp = ra_alloc_itemp();
//     if (ir1_opnd_is_gpr(ir1_get_opnd(pir1, 1))) {
//         src_opnd_1 = ra_alloc_gpr(ir1_opnd_base_reg_num(ir1_get_opnd(pir1, 1)));
//     } else if (imm12 && ir1_opnd_is_uimm_within_12bit(ir1_get_opnd(pir1, 1))) {
//         if (ir1_opnd_is_gpr(ir1_get_opnd(pir1, 0))) {
//             src_opnd_0 =
//                 ra_alloc_gpr(ir1_opnd_base_reg_num(ir1_get_opnd(pir1, 0)));
//             if (ir1_need_calculate_any_flag(pir1)) {
//                 IR2_OPND src_opnd_1 = load_ireg_from_ir1(ir1_get_opnd(pir1, 1),
//                                                          SIGN_EXTENSION, false);
//                 generate_eflag_calculation(src_opnd_0, src_opnd_0, src_opnd_1,
//                                            pir1, true);
//             }
//             if (ir1_opnd_size(ir1_get_opnd(pir1, 0)) == 64)
//                 la_append_ir2_opnd2i(opcode_imm12, src_opnd_0, src_opnd_0,
//                                      ir1_get_opnd(pir1, 1)->imm);
//             else {
//                 la_append_ir2_opnd2i(opcode_imm12, tmp, src_opnd_0,
//                                      ir1_get_opnd(pir1, 1)->imm);
//                 if (ir1_opnd_size(ir1_get_opnd(pir1, 0)) == 32)
//                     la_append_ir2_opnd2ii(LISA_BSTRPICK_D, src_opnd_0, tmp, 31,
//                                           0);
//                 else
//                     la_append_ir2_opnd2ii(
//                         LISA_BSTRINS_D, src_opnd_0, tmp,
//                         ir1_opnd_size(ir1_get_opnd(pir1, 0)) - 1, 0);
//             }
//         } else {
//             src_opnd_0 = load_ireg_from_ir1(ir1_get_opnd(pir1, 0),
//                                             SIGN_EXTENSION, false);
//             if (ir1_need_calculate_any_flag(pir1)) {
//                 IR2_OPND src_opnd_1 = load_ireg_from_ir1(ir1_get_opnd(pir1, 1),
//                                                          SIGN_EXTENSION, false);
//                 generate_eflag_calculation(src_opnd_0, src_opnd_0, src_opnd_1,
//                                            pir1, true);
//             }
//             la_append_ir2_opnd2i(opcode_imm12, tmp, src_opnd_0,
//                                  ir1_get_opnd(pir1, 1)->imm);
//             store_ireg_to_ir1(tmp, ir1_get_opnd(pir1, 0), false);
//         }
//         ra_free_temp(tmp);
//         return true;
//     } else {
//         src_opnd_1 =
//             load_ireg_from_ir1(ir1_get_opnd(pir1, 1), SIGN_EXTENSION, false);
//     }

//     if (ir1_opnd_is_gpr(ir1_get_opnd(pir1, 0))) {
//         src_opnd_0 = ra_alloc_gpr(ir1_opnd_base_reg_num(ir1_get_opnd(pir1, 0)));
//         // src_opnd_0 = load_ireg_from_ir1(ir1_get_opnd(pir1, 0),
//         // UNKNOWN_EXTENSION, false);
//         generate_eflag_calculation(src_opnd_0, src_opnd_0, src_opnd_1, pir1,
//                                    true);
//         if (ir1_opnd_size(ir1_get_opnd(pir1, 0)) == 64)
//             la_append_ir2_opnd3(opcode, src_opnd_0, src_opnd_0, src_opnd_1);
//         else {
//             la_append_ir2_opnd3(opcode, tmp, src_opnd_0, src_opnd_1);
//             store_ireg_to_ir1(tmp, ir1_get_opnd(pir1, 0), false);
//         }
//     } else {
//         src_opnd_0 =
//             load_ireg_from_ir1(ir1_get_opnd(pir1, 0), SIGN_EXTENSION, false);
//         la_append_ir2_opnd3_em(opcode, tmp, src_opnd_0, src_opnd_1);
//         generate_eflag_calculation(tmp, src_opnd_0, src_opnd_1, pir1, true);
//         store_ireg_to_ir1(tmp, ir1_get_opnd(pir1, 0), false);
//     }
//     ra_free_temp(tmp);
//     return true;
// }

bool translate_xor(IR1_INST *pir1)
{
    IR2_OPND lat_lock_addr;
    if (ir1_is_prefix_lock(pir1)) {
        IR2_OPND mem_opnd = mem_ir1_to_ir2_opnd(ir1_get_opnd(pir1, 0), false);
        int imm = ir2_opnd_imm(&mem_opnd);
        mem_opnd._type = IR2_OPND_IREG;
        lat_lock_addr = tr_lat_spin_lock(mem_opnd, imm);
    }
   
    IR1_OPND_TYPE opnd0_type = ir1_opnd_type(ir1_get_opnd(pir1, 0));
    IR1_OPND_TYPE opnd1_type = ir1_opnd_type(ir1_get_opnd(pir1, 1));
    switch (opnd0_type)
    {
    case X86_OP_REG:
        switch (opnd1_type) {
        case X86_OP_REG:
            if(unlikely(ir1_opnd_is_8h(ir1_get_opnd(pir1, 0)) || ir1_opnd_is_8h(ir1_get_opnd(pir1, 1))))
                reg_reg(pir1, LISA_XOR);
            else if ((ir1_get_opnd(pir1, 0)->reg == ir1_get_opnd(pir1, 1)->reg)) {
                IR2_OPND src_opnd_0 =
                    ra_alloc_gpr(ir1_opnd_base_reg_num(ir1_get_opnd(pir1, 0)));
                IR2_OPND src_opnd_1 =
                    ra_alloc_gpr(ir1_opnd_base_reg_num(ir1_get_opnd(pir1, 1)));
                generate_eflag_calculation(src_opnd_0, src_opnd_0, src_opnd_1,
                                           pir1, true);
                if (ir1_opnd_size(ir1_get_opnd(pir1, 0)) >= 32)
                    la_append_ir2_opnd2ii(LISA_BSTRINS_D, src_opnd_0,
                                          zero_ir2_opnd, 63, 0);
                else
                    la_append_ir2_opnd2ii(
                        LISA_BSTRINS_D, src_opnd_0, zero_ir2_opnd,
                        ir1_opnd_size(ir1_get_opnd(pir1, 0)) - 1, 0);
            } else
                reg_reg(pir1, LISA_XOR);
            break;

        case X86_OP_IMM:
            reg_imm(pir1, LISA_XOR, LISA_XORI, true);
            break;

        case X86_OP_MEM:
            reg_mem(pir1, LISA_XOR);
            break;

        default:
            universial(pir1, LISA_XOR);
            break;
        }

        break;
    
    case X86_OP_MEM:
        switch (opnd1_type)
        {
        case X86_OP_REG:
            mem_reg(pir1, LISA_XOR);
            break;
        
        case X86_OP_IMM:
            mem_imm(pir1, LISA_XOR, LISA_XORI, true);
            break;

        default:
            universial(pir1, LISA_XOR);
            break;
        }
        break;

    default:
        universial(pir1, LISA_XOR);
        break;
    }
    if (ir1_is_prefix_lock(pir1))
        tr_lat_spin_unlock(lat_lock_addr);
    return true;
}

bool translate_and(IR1_INST *pir1)
{
    IR2_OPND lat_lock_addr;
    if (ir1_is_prefix_lock(pir1)) {
        IR2_OPND mem_opnd = mem_ir1_to_ir2_opnd(ir1_get_opnd(pir1, 0), false);
        int imm = ir2_opnd_imm(&mem_opnd);
        mem_opnd._type = IR2_OPND_IREG;
        lat_lock_addr = tr_lat_spin_lock(mem_opnd, imm);
    }
    
    IR1_OPND_TYPE opnd0_type = ir1_opnd_type(ir1_get_opnd(pir1, 0));
    IR1_OPND_TYPE opnd1_type = ir1_opnd_type(ir1_get_opnd(pir1, 1));
    switch (opnd0_type)
    {
    case X86_OP_REG:
        switch (opnd1_type)
        {
        case X86_OP_REG:
            reg_reg(pir1, LISA_AND);
            break;
        
        case X86_OP_IMM:
            reg_imm(pir1, LISA_AND, LISA_ANDI, true);
            break;

        case X86_OP_MEM:
            reg_mem(pir1, LISA_AND);
            break;

        default:
            universial(pir1, LISA_AND);
            break;
        }

        break;
    
    case X86_OP_MEM:
        switch (opnd1_type)
        {
        case X86_OP_REG:
            mem_reg(pir1, LISA_AND);
            break;
        
        case X86_OP_IMM:
            mem_imm(pir1, LISA_AND, LISA_ANDI, true);
            break;

        default:
            universial(pir1, LISA_AND);
            break;
        }
        break;

    default:
        universial(pir1, LISA_AND);
        break;
    }

    if (ir1_is_prefix_lock(pir1)) {
        tr_lat_spin_unlock(lat_lock_addr);
    }
    return true;
}


bool translate_test(IR1_INST *pir1)
{
    IR2_OPND src_opnd_0 =
        load_ireg_from_ir1(ir1_get_opnd(pir1, 0), UNKNOWN_EXTENSION, false);
    IR2_OPND src_opnd_1 =
        load_ireg_from_ir1(ir1_get_opnd(pir1, 1), UNKNOWN_EXTENSION, false);

    generate_eflag_calculation(src_opnd_1, src_opnd_0, src_opnd_1, pir1, true);
    return true;
}

bool translate_or(IR1_INST *pir1)
{
    IR2_OPND lat_lock_addr;
    if (ir1_is_prefix_lock(pir1)) {
        IR2_OPND mem_opnd = mem_ir1_to_ir2_opnd(ir1_get_opnd(pir1, 0), false);
        int imm = ir2_opnd_imm(&mem_opnd);
        mem_opnd._type = IR2_OPND_IREG;
        lat_lock_addr = tr_lat_spin_lock(mem_opnd, imm);
    }
    IR1_OPND_TYPE opnd0_type = ir1_opnd_type(ir1_get_opnd(pir1, 0));
    IR1_OPND_TYPE opnd1_type = ir1_opnd_type(ir1_get_opnd(pir1, 1));
    switch (opnd0_type)
    {
    case X86_OP_REG:
        switch (opnd1_type)
        {
        case X86_OP_REG:
            reg_reg(pir1, LISA_OR);
            break;
        
        case X86_OP_IMM:
            reg_imm(pir1, LISA_OR, LISA_ORI, true);
            break;

        case X86_OP_MEM:
            reg_mem(pir1, LISA_OR);
            break;

        default:
            universial(pir1, LISA_OR);
            break;
        }

        break;
    
    case X86_OP_MEM:
        switch (opnd1_type)
        {
        case X86_OP_REG:
            mem_reg(pir1, LISA_OR);
            break;
        
        case X86_OP_IMM:
            mem_imm(pir1, LISA_OR, LISA_ORI, true);
            break;

        default:
            universial(pir1, LISA_OR);
            break;
        }
        break;

    default:
        universial(pir1, LISA_OR);
        break;
    }
    
    if (ir1_is_prefix_lock(pir1)) {
        tr_lat_spin_unlock(lat_lock_addr);
    }

    return true;
}

bool translate_not(IR1_INST *pir1)
{
    IR2_OPND src_opnd_0 =
        load_ireg_from_ir1(ir1_get_opnd(pir1, 0), SIGN_EXTENSION, false);

    IR2_OPND lat_lock_addr;

    if (ir1_is_prefix_lock(pir1)) {
        IR2_OPND mem_opnd = mem_ir1_to_ir2_opnd(ir1_get_opnd(pir1, 0), false);
        int imm = ir2_opnd_imm(&mem_opnd);
        mem_opnd._type = IR2_OPND_IREG;
        lat_lock_addr = tr_lat_spin_lock(mem_opnd, imm);
    }

#ifndef TARGET_X86_64
    IR2_OPND dest_opnd = ra_alloc_itemp();
    la_append_ir2_opnd3_em(LISA_NOR, dest_opnd, zero_ir2_opnd, src_opnd_0);
    store_ireg_to_ir1(dest_opnd, ir1_get_opnd(pir1, 0), false);
    ra_free_temp(dest_opnd);
#else
    if (ir1_opnd_size(ir1_get_opnd(pir1, 0)) == 64
        && ir1_opnd_is_gpr(ir1_get_opnd(pir1, 0))) {
        int reg_num = ir1_opnd_base_reg_num(ir1_get_opnd(pir1, 0));
        IR2_OPND dest_opnd = ra_alloc_gpr(reg_num);
        la_append_ir2_opnd3_em(LISA_NOR, dest_opnd, zero_ir2_opnd, src_opnd_0);
    } else {
        IR2_OPND dest_opnd = ra_alloc_itemp();
        la_append_ir2_opnd3_em(LISA_NOR, dest_opnd, zero_ir2_opnd, src_opnd_0);
        store_ireg_to_ir1(dest_opnd, ir1_get_opnd(pir1, 0), false);
        ra_free_temp(dest_opnd);
    }
#endif
    if (ir1_is_prefix_lock(pir1)) {
        tr_lat_spin_unlock(lat_lock_addr);
    }

    return true;
}

bool translate_shl(IR1_INST *pir1)
{
    IR2_OPND src =
        load_ireg_from_ir1(ir1_get_opnd(pir1, 0), SIGN_EXTENSION, false);
    IR2_OPND original_count =
        load_ireg_from_ir1(ir1_get_opnd(pir1, 1), ZERO_EXTENSION, false);
    IR2_OPND dest = ra_alloc_itemp();
    IR2_OPND count = ra_alloc_itemp();

#ifndef TARGET_X86_64
    la_append_ir2_opnd2i_em(LISA_ANDI, count, original_count, 31);
    IR2_OPND label_exit = ir2_opnd_new_type(IR2_OPND_LABEL);
    la_append_ir2_opnd3(LISA_BEQ, count, zero_ir2_opnd, label_exit);
    la_append_ir2_opnd3_em(LISA_SLL_W, dest, src, count);
#else
    int32 mask = (ir1_opnd_size(ir1_get_opnd(pir1, 0)) == 64) ? 63 : 31;
    la_append_ir2_opnd2i_em(LISA_ANDI, count, original_count, mask);
    IR2_OPND label_exit = ir2_opnd_new_type(IR2_OPND_LABEL);
    int opnd1_size = ir1_opnd_size(ir1_get_opnd(pir1, 0));
    if (opnd1_size == 32 && ir1_opnd_is_gpr(ir1_get_opnd(pir1, 0))) {
        /* no d_slot anymore, so exec before branch */
        IR2_OPND gpr_opnd =
            ra_alloc_gpr(ir1_opnd_base_reg_num(ir1_get_opnd(pir1, 0)));
        la_append_ir2_opnd2_em(LISA_MOV32_ZX, gpr_opnd, gpr_opnd);
        la_append_ir2_opnd3(LISA_BEQ, count, zero_ir2_opnd, label_exit);
    } else {
        la_append_ir2_opnd3(LISA_BEQ, count, zero_ir2_opnd, label_exit);
    }

    la_append_ir2_opnd3_em(LISA_SLL_D, dest, src, count);
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

bool translate_sal(IR1_INST *pir1)
{
#ifndef TARGET_X86_64
    IR2_OPND src = load_ireg_from_ir1(ir1_get_opnd(pir1, 0), SIGN_EXTENSION, false);
    IR2_OPND original_count =
        load_ireg_from_ir1(ir1_get_opnd(pir1, 0) + 1, ZERO_EXTENSION, false);
    IR2_OPND dest = ra_alloc_itemp();

    IR2_OPND count = ra_alloc_itemp();
    la_append_ir2_opnd2i_em(LISA_ANDI, count, original_count, 31);
    IR2_OPND label_exit = ir2_opnd_new_type(IR2_OPND_LABEL);
    la_append_ir2_opnd3(LISA_BEQ, count, zero_ir2_opnd, label_exit);

    la_append_ir2_opnd3_em(LISA_SLL_W, dest, src, count);

    generate_eflag_calculation(dest, src, count, pir1, true);

    store_ireg_to_ir1(dest, ir1_get_opnd(pir1, 0), false);

    la_append_ir2_opnd1(LISA_LABEL, label_exit);
    ra_free_temp(count);
    ra_free_temp(dest);
    return true;
#else
    return translate_shl(pir1);
#endif
}

bool translate_sar(IR1_INST *pir1)
{
    IR2_OPND src =
        load_ireg_from_ir1(ir1_get_opnd(pir1, 0), SIGN_EXTENSION, false);
    IR2_OPND original_count =
        load_ireg_from_ir1(ir1_get_opnd(pir1, 1), ZERO_EXTENSION, false);
    IR2_OPND dest = ra_alloc_itemp();
    IR2_OPND count = ra_alloc_itemp();
#ifndef TARGET_X86_64
    la_append_ir2_opnd2i_em(LISA_ANDI, count, original_count, 31);
    IR2_OPND label_exit = ir2_opnd_new_type(IR2_OPND_LABEL);
    la_append_ir2_opnd3(LISA_BEQ, count, zero_ir2_opnd, label_exit);
    la_append_ir2_opnd3(LISA_SRA_W, dest, src, count);
#else
    int32 mask = (ir1_opnd_size(ir1_get_opnd(pir1, 0)) == 64) ? 63 : 31;
    la_append_ir2_opnd2i_em(LISA_ANDI, count, original_count, mask);
    IR2_OPND label_exit = ir2_opnd_new_type(IR2_OPND_LABEL);
    int opnd1_size = ir1_opnd_size(ir1_get_opnd(pir1, 0));
    if (opnd1_size == 32
        && ir1_opnd_is_gpr(ir1_get_opnd(pir1, 0))) {
        /* no d_slot anymore, so exec before branch */
        IR2_OPND gpr_opnd = ra_alloc_gpr(ir1_opnd_base_reg_num(ir1_get_opnd(pir1, 0)));
        la_append_ir2_opnd2_em(LISA_MOV32_ZX, gpr_opnd, gpr_opnd);
        la_append_ir2_opnd3(LISA_BEQ, count, zero_ir2_opnd, label_exit);
    } else {
        la_append_ir2_opnd3(LISA_BEQ, count, zero_ir2_opnd, label_exit);
    }

    la_append_ir2_opnd3_em(LISA_SRA_D, dest, src, count);
#endif
    generate_eflag_calculation(dest, src, count, pir1, true);
    store_ireg_to_ir1(dest, ir1_get_opnd(pir1, 0), false);
    la_append_ir2_opnd1(LISA_LABEL, label_exit);
    ra_free_temp(count);
    ra_free_temp(dest);
    return true;
}

bool translate_rol(IR1_INST *pir1)
{
    IR1_OPND *opnd0 = ir1_get_opnd(pir1, 0);
    IR1_OPND *opnd1 = ir1_get_opnd(pir1, 1);
    int dest_size = ir1_opnd_size(opnd0);

    /* get real count */
    IR2_OPND label_exit = ir2_opnd_new_type(IR2_OPND_LABEL);
    IR2_OPND original_count =
        load_ireg_from_ir1(opnd1, ZERO_EXTENSION, false);
    IR2_OPND count = ra_alloc_itemp();
#ifndef TARGET_X86_64
    la_append_ir2_opnd2i_em(LISA_ANDI, count, original_count, 0x1f);
    la_append_ir2_opnd3(LISA_BEQ, count, zero_ir2_opnd, label_exit);
#else
    if (ir1_opnd_size(opnd0) == 64) {
        la_append_ir2_opnd2i_em(LISA_ANDI, count, original_count, 0x3f);
    } else {
        la_append_ir2_opnd2i_em(LISA_ANDI, count, original_count, 0x1f);
    }
    if (ir1_opnd_size(ir1_get_opnd(pir1, 0)) == 32
    && ir1_opnd_is_gpr(ir1_get_opnd(pir1, 0))) {
        /* clean the target*/
        IR2_OPND gpr_opnd = ra_alloc_gpr(ir1_opnd_base_reg_num(ir1_get_opnd(pir1, 0)));
        la_append_ir2_opnd2_em(LISA_MOV32_ZX, gpr_opnd, gpr_opnd);
        la_append_ir2_opnd3(LISA_BEQ, count, zero_ir2_opnd, label_exit);
    } else {
        la_append_ir2_opnd3(LISA_BEQ, count, zero_ir2_opnd, label_exit);
    }
#endif

    IR2_OPND dest = load_ireg_from_ir1(opnd0, ZERO_EXTENSION, false);
    IR2_OPND tmp_dest = ra_alloc_itemp();

    if (ir1_need_calculate_any_flag(pir1)) {
        if (ir1_opnd_size(opnd0) == 8) {
            la_append_ir2_opnd2(LISA_X86ROTL_B, dest, original_count);
        } else if (ir1_opnd_size(opnd0) == 16) {
            la_append_ir2_opnd2(LISA_X86ROTL_H, dest, original_count);
        } else if (ir1_opnd_size(opnd0) == 32) {
            la_append_ir2_opnd2(LISA_X86ROTL_W, dest, original_count);
        } else if (ir1_opnd_size(opnd0) == 64) {
            la_append_ir2_opnd2(LISA_X86ROTL_D, dest, original_count);
        }
    }

    if (dest_size != 32 && dest_size != 64) {
        la_append_ir2_opnd2i_em(LISA_ANDI, count, original_count,
                                dest_size - 1);
    }
    if (ir2_opnd_is_itemp(&original_count)) {
        ra_free_temp(original_count);
    }

    IR2_OPND tmp = ra_alloc_itemp();
    load_ireg_from_imm32(tmp, dest_size, ZERO_EXTENSION);
    la_append_ir2_opnd3_em(LISA_SUB_D, tmp, tmp, count);
    la_append_ir2_opnd3_em(LISA_ROTR_D, tmp_dest, dest, tmp);
#ifndef TARGET_X86_64
    la_append_ir2_opnd2i_em(LISA_SRLI_D, tmp, tmp_dest, 64 - dest_size);
    la_append_ir2_opnd3_em(LISA_OR, tmp_dest, tmp_dest, tmp);
    la_append_ir2_opnd2ii(LISA_BSTRINS_D, tmp_dest, zero_ir2_opnd, 63, 32);
#else
    if (dest_size != 64) {
        la_append_ir2_opnd2i_em(LISA_SRLI_D, tmp, tmp_dest, 64 - dest_size);
        la_append_ir2_opnd3_em(LISA_OR, tmp_dest, tmp_dest, tmp);
    }
#endif
    store_ireg_to_ir1(tmp_dest, ir1_get_opnd(pir1, 0), false);
    la_append_ir2_opnd1(LISA_LABEL, label_exit);
    ra_free_temp(tmp);
    ra_free_temp(tmp_dest);
    return true;
}

bool translate_ror(IR1_INST *pir1)
{
    IR1_OPND *opnd0 = ir1_get_opnd(pir1, 0);
    IR1_OPND *opnd1 = ir1_get_opnd(pir1, 1);
    int dest_size = ir1_opnd_size(opnd0);

    /* get real count */
    IR2_OPND label_exit = ir2_opnd_new_type(IR2_OPND_LABEL);
    IR2_OPND original_count =
        load_ireg_from_ir1(opnd1, ZERO_EXTENSION, false);
    IR2_OPND count = ra_alloc_itemp();
#ifndef TARGET_X86_64
    la_append_ir2_opnd2i_em(LISA_ANDI, count, original_count, 0x1f);
    la_append_ir2_opnd3(LISA_BEQ, count, zero_ir2_opnd, label_exit);
#else
    if (ir1_opnd_size(ir1_get_opnd(pir1, 0)) == 64) {
        la_append_ir2_opnd2i_em(LISA_ANDI, count, original_count, 0x3f);
    } else {
        la_append_ir2_opnd2i_em(LISA_ANDI, count, original_count, 0x1f);
    }
    if (ir1_opnd_size(ir1_get_opnd(pir1, 0)) == 32
    && ir1_opnd_is_gpr(ir1_get_opnd(pir1, 0))) {
        /* clean the target*/
        IR2_OPND gpr_opnd = ra_alloc_gpr(ir1_opnd_base_reg_num(ir1_get_opnd(pir1, 0)));
        la_append_ir2_opnd2_em(LISA_MOV32_ZX, gpr_opnd, gpr_opnd);
        la_append_ir2_opnd3(LISA_BEQ, count, zero_ir2_opnd, label_exit);
    } else {
        la_append_ir2_opnd3(LISA_BEQ, count, zero_ir2_opnd, label_exit);
    }
#endif
    IR2_OPND dest = load_ireg_from_ir1(opnd0, ZERO_EXTENSION, false);
    IR2_OPND tmp_dest = ra_alloc_itemp();
    IR2_OPND tmp = ra_alloc_itemp();

    if (ir1_need_calculate_any_flag(pir1)) {
        if (ir1_opnd_size(ir1_get_opnd(pir1, 0)) == 8) {
            la_append_ir2_opnd2(LISA_X86ROTR_B, dest, original_count);
        } else if (ir1_opnd_size(ir1_get_opnd(pir1, 0)) == 16) {
            la_append_ir2_opnd2(LISA_X86ROTR_H, dest, original_count);
        } else if (ir1_opnd_size(ir1_get_opnd(pir1, 0)) == 32) {
            la_append_ir2_opnd2(LISA_X86ROTR_W, dest, original_count);
        } else if (ir1_opnd_size(ir1_get_opnd(pir1, 0)) == 64) {
            la_append_ir2_opnd2(LISA_X86ROTR_D, dest, original_count);
        }
    }

    if (dest_size != 32 && dest_size != 64) {
        la_append_ir2_opnd2i_em(LISA_ANDI, count, original_count, dest_size - 1);
    }
    if (ir2_opnd_is_itemp(&original_count)) {
        ra_free_temp(original_count);
    }

    la_append_ir2_opnd3_em(LISA_ROTR_D, tmp_dest, dest, count);
#ifndef TARGET_X86_64
    la_append_ir2_opnd2i_em(LISA_SRLI_D, tmp, tmp_dest, 64 - dest_size);
    la_append_ir2_opnd3_em(LISA_OR, tmp_dest, tmp_dest, tmp);
    la_append_ir2_opnd2ii(LISA_BSTRINS_D, tmp_dest, zero_ir2_opnd, 63, 32);
#else
    if (dest_size != 64) {
        la_append_ir2_opnd2i_em(LISA_SRLI_D, tmp, tmp_dest, 64 - dest_size);
        la_append_ir2_opnd3_em(LISA_OR, tmp_dest, tmp_dest, tmp);
    }
#endif
    store_ireg_to_ir1(tmp_dest, ir1_get_opnd(pir1, 0), false);
    la_append_ir2_opnd1(LISA_LABEL, label_exit);
    ra_free_temp(tmp);
    ra_free_temp(tmp_dest);
    return true;
}

bool translate_rcl(IR1_INST *pir1)
{
    IR1_OPND *opnd0 = ir1_get_opnd(pir1, 0);
    int dest_size = ir1_opnd_size(opnd0);

    /* get real count */
    IR2_OPND label_exit = ir2_opnd_new_type(IR2_OPND_LABEL);

#ifdef CONFIG_CAPSTONE_NEXT
    IR2_OPND original_count = ra_alloc_itemp();
    if (pir1->info->detail->x86.op_count == 1)
            la_append_ir2_opnd2i_em(LISA_ORI, original_count, zero_ir2_opnd, 0x1);
    else
        original_count =
            load_ireg_from_ir1(ir1_get_opnd(pir1, 0) + 1, ZERO_EXTENSION, false);
#else
    IR1_OPND *opnd1 = ir1_get_opnd(pir1, 1);
    IR2_OPND original_count =
        load_ireg_from_ir1(opnd1, ZERO_EXTENSION, false);
#endif

    IR2_OPND count = ra_alloc_itemp();
#ifndef TARGET_X86_64
    la_append_ir2_opnd2i_em(LISA_ANDI, count, original_count, 0x1f);
    la_append_ir2_opnd3(LISA_BEQ, count, zero_ir2_opnd, label_exit);
#else
    int32 mask = (dest_size == 64) ? 63 : 31;
    la_append_ir2_opnd2i_em(LISA_ANDI, count, original_count, mask);
    if (dest_size == 32 && ir1_opnd_is_gpr(ir1_get_opnd(pir1, 0))) {
        /* clean the target*/
        IR2_OPND gpr_opnd =
            ra_alloc_gpr(ir1_opnd_base_reg_num(ir1_get_opnd(pir1, 0)));
        la_append_ir2_opnd2_em(LISA_MOV32_ZX, gpr_opnd, gpr_opnd);
        la_append_ir2_opnd3(LISA_BEQ, count, zero_ir2_opnd, label_exit);
    } else {
        la_append_ir2_opnd3(LISA_BEQ, count, zero_ir2_opnd, label_exit);
    }
#endif
    if (ir2_opnd_is_itemp(&original_count)) {
        ra_free_temp(original_count);
    }

    if (dest_size < 32) {
        IR2_OPND tmp_imm = ra_alloc_itemp();
        la_append_ir2_opnd2i_em(LISA_ADDI_D, tmp_imm, zero_ir2_opnd,
                          dest_size + 1);
        la_append_ir2_opnd3(LISA_MOD_DU, count, count, tmp_imm);
        la_append_ir2_opnd3(LISA_BEQ, count, zero_ir2_opnd, label_exit);
        ra_free_temp(tmp_imm);
    }

    IR2_OPND dest = load_ireg_from_ir1(opnd0, ZERO_EXTENSION, false);
#ifndef TARGET_X86_64
    IR2_OPND tmp_dest = ra_alloc_itemp();

    get_eflag_condition(&tmp_dest, pir1);
    la_append_ir2_opnd2i_em(LISA_SLLI_D, tmp_dest, tmp_dest, dest_size);
    la_append_ir2_opnd2ii(LISA_BSTRINS_D, tmp_dest, dest, dest_size - 1, 0);

    IR2_OPND tmp = ra_alloc_itemp();
    load_ireg_from_imm32(tmp, dest_size + 1, ZERO_EXTENSION);
    la_append_ir2_opnd3_em(LISA_SUB_D, tmp, tmp, count);
    la_append_ir2_opnd3_em(LISA_ROTR_D, tmp, tmp_dest, tmp);
    la_append_ir2_opnd2i_em(LISA_SRLI_D, tmp_dest, tmp, 63 - dest_size);
    la_append_ir2_opnd3_em(LISA_OR, tmp_dest, tmp_dest, tmp);
    la_append_ir2_opnd2ii(LISA_BSTRINS_D, tmp_dest, zero_ir2_opnd, 63, 32);
    generate_eflag_calculation(tmp_dest, dest, count, pir1, true);
    store_ireg_to_ir1(tmp_dest, ir1_get_opnd(pir1, 0), false);
    ra_free_temp(tmp);
    ra_free_temp(tmp_dest);
#else
    IR2_OPND cf = ra_alloc_itemp();
    get_eflag_condition(&cf, pir1);
    if (dest_size <= 32) {
        la_append_ir2_opnd2i_em(LISA_SLLI_D, cf, cf, dest_size);
    } else {
        IR2_OPND cf_move = ra_alloc_itemp();
        /* 64 bits, cf will put the right bit now */
        /* cf_move <- count - 1 */
        la_append_ir2_opnd2i_em(LISA_SUBIU, cf_move, count, 1);
        /* cf <- cf << cf_move */
        la_append_ir2_opnd3_em(LISA_SLL_D, cf, cf, cf_move);
        ra_free_temp(cf_move);
    }

    IR2_OPND tmp_dest;
    IR2_OPND high_dest = ra_alloc_itemp();
    IR2_OPND label_finish = ir2_opnd_new_type(IR2_OPND_LABEL);
    /* but if size = 64, no more bits for the cf */
    if (dest_size != 64) {
        tmp_dest = ra_alloc_itemp();
        /* for 32bits etc. we can put the cf at the upper bit */
        la_append_ir2_opnd3_em(LISA_OR, tmp_dest, dest, cf);
        la_append_ir2_opnd3_em(LISA_SLL_D, high_dest, tmp_dest, count);
    } else {
        la_append_ir2_opnd3_em(LISA_SLL_D, high_dest, dest, count);
        /* attach the cf at the high_dest */
        la_append_ir2_opnd3_em(LISA_OR, high_dest, high_dest, cf);
        la_append_ir2_opnd2i_em(LISA_SUBIU, cf, count, 1);
        la_append_ir2_opnd3(LISA_BEQ, cf, zero_ir2_opnd, label_finish);
        tmp_dest = dest;
    }
    ra_free_temp(cf);

    la_append_ir2_opnd2i_em(LISA_ADDI_D, count, count, -1 - dest_size);
    la_append_ir2_opnd3_em(LISA_SUB_D, count, zero_ir2_opnd, count);

    IR2_OPND low_dest = ra_alloc_itemp();
    la_append_ir2_opnd3_em(LISA_SRL_D, low_dest, tmp_dest, count);
    la_append_ir2_opnd3_em(LISA_SUB_D, count, zero_ir2_opnd, count);
    la_append_ir2_opnd2i_em(LISA_ADDI_D, count, count, 1 + dest_size);

    if (dest_size != 64) {
        ra_free_temp(tmp_dest);
    }

    la_append_ir2_opnd3_em(LISA_OR, high_dest, high_dest, low_dest);
/* label_finish: */
    la_append_ir2_opnd1(LISA_LABEL, label_finish);
    generate_eflag_calculation(high_dest, dest, count, pir1, true);
    /* we can't move the dest directly */
    /* because eflag_calculation need the resource data */
    store_ireg_to_ir1(high_dest, ir1_get_opnd(pir1, 0), false);
    ra_free_temp(high_dest);
    ra_free_temp(low_dest);
    ra_free_temp(count);
#endif
/* label_exit: */
    la_append_ir2_opnd1(LISA_LABEL, label_exit);
    return true;
}

bool translate_rcr(IR1_INST *pir1)
{
    IR1_OPND *opnd0 = ir1_get_opnd(pir1, 0);
    IR1_OPND *opnd1 = ir1_get_opnd(pir1, 1);
    int dest_size = ir1_opnd_size(opnd0);

    /* get real count */
    IR2_OPND label_exit = ir2_opnd_new_type(IR2_OPND_LABEL);
    IR2_OPND original_count =
        load_ireg_from_ir1(opnd1, ZERO_EXTENSION, false);
    IR2_OPND count = ra_alloc_itemp();
#ifndef TARGET_X86_64
    la_append_ir2_opnd2i_em(LISA_ANDI, count, original_count, 0x1f);
    la_append_ir2_opnd3(LISA_BEQ, count, zero_ir2_opnd, label_exit);
#else
    int32 mask = (dest_size == 64) ? 63 : 31;
    la_append_ir2_opnd2i_em(LISA_ANDI, count, original_count, mask);
    if (dest_size == 32 && ir1_opnd_is_gpr(ir1_get_opnd(pir1, 0))) {
        /* clean the target*/
        IR2_OPND gpr_opnd =
            ra_alloc_gpr(ir1_opnd_base_reg_num(ir1_get_opnd(pir1, 0)));
        la_append_ir2_opnd2_em(LISA_MOV32_ZX, gpr_opnd, gpr_opnd);
        la_append_ir2_opnd3(LISA_BEQ, count, zero_ir2_opnd, label_exit);
    } else {
        la_append_ir2_opnd3(LISA_BEQ, count, zero_ir2_opnd, label_exit);
    }
#endif

    if (ir1_opnd_size(opnd0) < 32) {
        IR2_OPND tmp_imm = ra_alloc_itemp();
        la_append_ir2_opnd2i_em(LISA_ADDI_D, tmp_imm, zero_ir2_opnd,
                          dest_size + 1);
        la_append_ir2_opnd3(LISA_MOD_DU, count, count, tmp_imm);
        la_append_ir2_opnd3(LISA_BEQ, count, zero_ir2_opnd, label_exit);
        ra_free_temp(tmp_imm);
    }

    IR2_OPND dest = load_ireg_from_ir1(opnd0, ZERO_EXTENSION, false);
#ifndef TARGET_X86_64
    IR2_OPND tmp_dest = ra_alloc_itemp();

    get_eflag_condition(&tmp_dest, pir1);
    la_append_ir2_opnd2ii(LISA_BSTRINS_D, tmp_dest, dest, dest_size, 1);

    IR2_OPND tmp = ra_alloc_itemp();
    la_append_ir2_opnd3_em(LISA_ROTR_D, tmp, tmp_dest, count);
    la_append_ir2_opnd2i_em(LISA_SRLI_D, tmp_dest, tmp, 63 - dest_size);
    la_append_ir2_opnd3_em(LISA_OR, tmp_dest, tmp_dest, tmp);

    la_append_ir2_opnd2ii(LISA_BSTRPICK_D, tmp_dest, tmp_dest, dest_size, 1);
    generate_eflag_calculation(tmp_dest, dest, count, pir1, true);
    store_ireg_to_ir1(tmp_dest, ir1_get_opnd(pir1, 0), false);
    ra_free_temp(tmp);
    ra_free_temp(tmp_dest);
#else
    IR2_OPND cf = ra_alloc_itemp();
    get_eflag_condition(&cf, pir1);
    if (dest_size <= 32) {
        la_append_ir2_opnd2i_em(LISA_SLLI_D, cf, cf, dest_size);
    } else {
        IR2_OPND cf_move = ra_alloc_itemp();
        /* 64 bits, cf will put the right bit now */
        /* cf_move <- size - count */
        la_append_ir2_opnd2i_em(LISA_ADDI_D, cf_move, zero_ir2_opnd, 64);
        la_append_ir2_opnd3_em(LISA_SUB_W, cf_move, cf_move, count);
        /* cf <- cf << cf_move */
        la_append_ir2_opnd3_em(LISA_SLL_D, cf, cf, cf_move);
        ra_free_temp(cf_move);
    }

    IR2_OPND tmp_dest;
    IR2_OPND low_dest = ra_alloc_itemp();
    IR2_OPND label_finish = ir2_opnd_new_type(IR2_OPND_LABEL);

    /* but if size = 64, no more bits for the cf */
    if (dest_size != 64) {
        tmp_dest = ra_alloc_itemp();
        /* for 32bits etc. we can put the cf at the upper bit */
        la_append_ir2_opnd3_em(LISA_OR, tmp_dest, dest, cf);
        la_append_ir2_opnd3_em(LISA_SRL_D, low_dest, tmp_dest, count);
    } else {
        la_append_ir2_opnd3_em(LISA_SRL_D, low_dest, dest, count);
        /* if count = 1 */
        /* attach the cf at the low_dest */
        la_append_ir2_opnd3_em(LISA_OR, low_dest, low_dest, cf);
        la_append_ir2_opnd2i_em(LISA_SUBIU, cf, count, 1);
        la_append_ir2_opnd3(LISA_BEQ, cf, zero_ir2_opnd, label_finish);
        tmp_dest = dest;
    }
    ra_free_temp(cf);

    /* attention: dsllv $reg, $reg(=64) is not work */
    la_append_ir2_opnd2i_em(LISA_ADDI_D, count, count, -1 - dest_size);
    la_append_ir2_opnd3_em(LISA_SUB_D, count, zero_ir2_opnd, count);

    IR2_OPND high_dest = ra_alloc_itemp();
    la_append_ir2_opnd3_em(LISA_SLL_D, high_dest, tmp_dest, count);
    la_append_ir2_opnd3_em(LISA_SUB_D, count, zero_ir2_opnd, count);
    la_append_ir2_opnd2i_em(LISA_ADDI_D, count, count, 1 + dest_size);

    if (dest_size != 64) {
        ra_free_temp(tmp_dest);
    }

    la_append_ir2_opnd3_em(LISA_OR, low_dest, high_dest, low_dest);
    ra_free_temp(high_dest);
/* label_finish: */
    la_append_ir2_opnd1(LISA_LABEL, label_finish);
    generate_eflag_calculation(low_dest, dest, count, pir1, true);
    ra_free_temp(count);
    store_ireg_to_ir1(low_dest, ir1_get_opnd(pir1, 0), false);
    ra_free_temp(low_dest);
#endif
/* label_exit: */
    la_append_ir2_opnd1(LISA_LABEL, label_exit);
    return true;
}

static bool translate_shrd_cl(IR1_INST *pir1)
{
    IR2_OPND count_opnd =
        load_ireg_from_ir1(ir1_get_opnd(pir1, 0) + 2, ZERO_EXTENSION, false);

    IR2_OPND count = ra_alloc_itemp();
    int opnd0_size = ir1_opnd_size(ir1_get_opnd(pir1, 0));
    int mask = (opnd0_size == 64) ? 0x3f : 0x1f;
    la_append_ir2_opnd2i_em(LISA_ANDI, count, count_opnd, mask);
    if (ir2_opnd_is_itemp(&count_opnd)) {
        ra_free_temp(count_opnd);
    }
    IR2_OPND label_exit = ir2_opnd_new_type(IR2_OPND_LABEL);
    /* if reg size is 32 and dest is not 32 zero extend */
#ifdef TARGET_X86_64
    if (ir1_opnd_is_gpr(ir1_get_opnd(pir1, 0)) &&
        opnd0_size == 32) {
        /* clean the target */
        IR2_OPND dest_opnd =
                ra_alloc_gpr(ir1_opnd_base_reg_num(ir1_get_opnd(pir1, 0)));
        la_append_ir2_opnd2_em(LISA_CLR_H32, dest_opnd, dest_opnd);
    }
#endif
    la_append_ir2_opnd3(LISA_BEQ, count, zero_ir2_opnd, label_exit);

    IR2_OPND left_count = ra_alloc_itemp();
    load_ireg_from_imm32(left_count, mask + 1, SIGN_EXTENSION);
    la_append_ir2_opnd3_em(LISA_SUB_W, left_count, left_count, count);

    IR2_OPND dest_opnd = ra_alloc_itemp();
    IR2_OPND src_opnd = ra_alloc_itemp();
    /* use temp register to avoid clobber if src == dest */
    load_ireg_from_ir1_2(dest_opnd, ir1_get_opnd(pir1, 0), ZERO_EXTENSION, false);
    load_ireg_from_ir1_2(src_opnd, ir1_get_opnd(pir1, 1), ZERO_EXTENSION, false);
    /* TODO: 32 bit case in X86_64 */
    if (opnd0_size == 16) {
        la_append_ir2_opnd2ii(LISA_BSTRINS_D, dest_opnd, src_opnd, 31, 16);
        la_append_ir2_opnd2ii(LISA_BSTRINS_D, src_opnd, dest_opnd, 15, 0);
    }
    IR2_OPND low_dest = ra_alloc_itemp();
    la_append_ir2_opnd3_em(LISA_SRL_D, low_dest, dest_opnd, count);
    la_append_ir2_opnd3_em(LISA_SLL_D, src_opnd, src_opnd, left_count);
    ra_free_temp(left_count);

    IR2_OPND final_dest = ra_alloc_itemp();
    la_append_ir2_opnd3_em(LISA_OR, final_dest, src_opnd, low_dest);
    ra_free_temp(low_dest);
    ra_free_temp(src_opnd);

    generate_eflag_calculation(final_dest, dest_opnd, count, pir1, true);
    ra_free_temp(dest_opnd);
    ra_free_temp(count);

    store_ireg_to_ir1(final_dest, ir1_get_opnd(pir1, 0), false);

    ra_free_temp(final_dest);
    la_append_ir2_opnd1(LISA_LABEL, label_exit);

    return true;
}

static bool translate_shrd_imm(IR1_INST *pir1)
{
    lsassert(ir1_opnd_is_imm(ir1_get_opnd(pir1, 0) + 2));
    int opnd0_size = ir1_opnd_size(ir1_get_opnd(pir1, 0));
    int32 mask = (opnd0_size == 64) ? 0x3f : 0x1f;
    int count = ir1_opnd_simm(ir1_get_opnd(pir1, 2)) & mask;

    if (count == 0) {
#ifdef TARGET_X86_64
        if (ir1_opnd_is_gpr(ir1_get_opnd(pir1, 0))
            && opnd0_size == 32) {
            /* clean high 32 bits */
            IR2_OPND dest_opnd =
                ra_alloc_gpr(ir1_opnd_base_reg_num(ir1_get_opnd(pir1, 0)));
            la_append_ir2_opnd2_em(LISA_CLR_H32, dest_opnd, dest_opnd);
        }
#endif
        return true;
    }
    int left_count = mask + 1 - count;

    IR2_OPND src_opnd = ra_alloc_itemp();
    IR2_OPND dest_opnd = ra_alloc_itemp();
    /* use temp register to avoid clobber if src == dest */
    load_ireg_from_ir1_2(dest_opnd, ir1_get_opnd(pir1, 0), ZERO_EXTENSION, false);
    load_ireg_from_ir1_2(src_opnd, ir1_get_opnd(pir1, 1), ZERO_EXTENSION, false);
    /* TODO: 32 bit case in X86_64 */
    if (opnd0_size == 16) {
        la_append_ir2_opnd2ii(LISA_BSTRINS_D, dest_opnd, src_opnd, 31, 16);
        la_append_ir2_opnd2ii(LISA_BSTRINS_D, src_opnd, dest_opnd, 15, 0);
    }
    /* shift right firest */
    IR2_OPND low_dest = ra_alloc_itemp();
    IR2_OPND high_dest = ra_alloc_itemp();
    la_append_ir2_opnd2i_em(LISA_SRLI_D, low_dest, dest_opnd, count);
    la_append_ir2_opnd2i_em(LISA_SLLI_D, high_dest, src_opnd, left_count);
    ra_free_temp(src_opnd);

    IR2_OPND final_dest = ra_alloc_itemp();
    la_append_ir2_opnd3_em(LISA_OR, final_dest, high_dest, low_dest);
    ra_free_temp(low_dest);
    ra_free_temp(high_dest);

    IR2_OPND count_opnd = ir2_opnd_new(IR2_OPND_IMM, (int16)count);
    generate_eflag_calculation(final_dest, dest_opnd, count_opnd, pir1, true);

    store_ireg_to_ir1(final_dest, ir1_get_opnd(pir1, 0), false);
    ra_free_temp(final_dest);

    return true;
}

bool translate_shrd(IR1_INST *pir1)
{
    if (ir1_opnd_is_imm(ir1_get_opnd(pir1, 0) + 2))
        return translate_shrd_imm(pir1);
    else
        return translate_shrd_cl(pir1);

    return true;
}

static bool translate_shld_cl(IR1_INST *pir1)
{
    IR2_OPND count_opnd =
        load_ireg_from_ir1(ir1_get_opnd(pir1, 0) + 2, ZERO_EXTENSION, false);

    IR2_OPND count = ra_alloc_itemp();
    int opnd0_size = ir1_opnd_size(ir1_get_opnd(pir1, 0));
    int mask = (opnd0_size == 64) ? 63 : 31;
    la_append_ir2_opnd2i_em(LISA_ANDI, count, count_opnd, mask);
    if (ir2_opnd_is_itemp(&count_opnd)) {
        ra_free_temp(count_opnd);
    }
    IR2_OPND label_exit = ir2_opnd_new_type(IR2_OPND_LABEL);
    /* if reg size is 32 and dest is not 32 zero extend */
#ifdef TARGET_X86_64
    if (ir1_opnd_is_gpr(ir1_get_opnd(pir1, 0)) &&
        opnd0_size == 32) {
        /* clean the target */
        IR2_OPND dest_opnd =
                ra_alloc_gpr(ir1_opnd_base_reg_num(ir1_get_opnd(pir1, 0)));
        la_append_ir2_opnd2_em(LISA_CLR_H32, dest_opnd, dest_opnd);
    }
#endif
    la_append_ir2_opnd3(LISA_BEQ, count, zero_ir2_opnd, label_exit);

    IR2_OPND left_count = ra_alloc_itemp();
    load_ireg_from_imm32(left_count, mask + 1, SIGN_EXTENSION);
    la_append_ir2_opnd3_em(LISA_SUB_W, left_count, left_count, count);

    IR2_OPND src_opnd = ra_alloc_itemp();
    IR2_OPND dest_opnd = ra_alloc_itemp();
    /* use temp register to avoid clobber if src == dest */
    load_ireg_from_ir1_2(dest_opnd, ir1_get_opnd(pir1, 0), ZERO_EXTENSION, false);
    load_ireg_from_ir1_2(src_opnd, ir1_get_opnd(pir1, 1), ZERO_EXTENSION, false);

    /* TODO: 32 bit case in X86_64 */
    if (opnd0_size == 16) {
        la_append_ir2_opnd2ii(LISA_BSTRINS_D, src_opnd, src_opnd, 31, 16);
        la_append_ir2_opnd2ii(LISA_BSTRINS_D, src_opnd, dest_opnd, 15, 0);
    }

    IR2_OPND high_dest = ra_alloc_itemp();
    la_append_ir2_opnd3_em(LISA_SLL_D, high_dest, dest_opnd, count);

    IR2_OPND low_dest = ra_alloc_itemp();
    la_append_ir2_opnd3_em(LISA_SRL_D, low_dest, src_opnd, left_count);
    ra_free_temp(left_count);

    IR2_OPND final_dest = ra_alloc_itemp();
    la_append_ir2_opnd3_em(LISA_OR, final_dest, high_dest, low_dest);
    ra_free_temp(low_dest);
    ra_free_temp(high_dest);
    if (opnd0_size == 16) {
        la_append_ir2_opnd2ii(LISA_BSTRINS_D, dest_opnd, dest_opnd, 31, 16);
        la_append_ir2_opnd2ii(LISA_BSTRPICK_D, src_opnd, src_opnd, 31, 16);
        la_append_ir2_opnd2ii(LISA_BSTRINS_D, dest_opnd, src_opnd, 15, 0);
    }
    ra_free_temp(src_opnd);

    generate_eflag_calculation(final_dest, dest_opnd, count, pir1, true);
    ra_free_temp(count);

    store_ireg_to_ir1(final_dest, ir1_get_opnd(pir1, 0), false);

    ra_free_temp(final_dest);
    la_append_ir2_opnd1(LISA_LABEL, label_exit);

    return true;
}

static bool translate_shld_imm(IR1_INST *pir1)
{
    lsassert(ir1_opnd_is_imm(ir1_get_opnd(pir1, 0) + 2));
    int opnd0_size = ir1_opnd_size(ir1_get_opnd(pir1, 0));
    int32 mask = (opnd0_size == 64) ? 0x3f : 0x1f;
    int count = ir1_opnd_simm(ir1_get_opnd(pir1, 2)) & mask;
    if (count == 0) {
#ifdef TARGET_X86_64
        if (ir1_opnd_is_gpr(ir1_get_opnd(pir1, 0))
            && opnd0_size == 32) {
            /* clean high 32 bits */
            IR2_OPND dest_opnd =
                ra_alloc_gpr(ir1_opnd_base_reg_num(ir1_get_opnd(pir1, 0)));
            la_append_ir2_opnd2_em(LISA_CLR_H32, dest_opnd, dest_opnd);
        }
#endif
        return true;
    }
    int left_count = mask + 1 - count;

    IR2_OPND src_opnd = ra_alloc_itemp();
    IR2_OPND dest_opnd = ra_alloc_itemp();

    /* use temp register to avoid clobber if src == dest */
    load_ireg_from_ir1_2(dest_opnd, ir1_get_opnd(pir1, 0), ZERO_EXTENSION, false);
    load_ireg_from_ir1_2(src_opnd, ir1_get_opnd(pir1, 1), ZERO_EXTENSION, false);

    /* TODO: 32 bit case in X86_64 */
    if (opnd0_size == 16) {
        la_append_ir2_opnd2ii(LISA_BSTRINS_D, src_opnd, src_opnd, 31, 16);
        la_append_ir2_opnd2ii(LISA_BSTRINS_D, src_opnd, dest_opnd, 15, 0);
    }

    IR2_OPND high_dest = ra_alloc_itemp();
    IR2_OPND low_dest = ra_alloc_itemp();
    la_append_ir2_opnd2i_em(LISA_SLLI_D, high_dest, dest_opnd, count);
    la_append_ir2_opnd2i_em(LISA_SRLI_D, low_dest, src_opnd, left_count);

    IR2_OPND final_dest = ra_alloc_itemp();
    la_append_ir2_opnd3_em(LISA_OR, final_dest, high_dest, low_dest);
    ra_free_temp(low_dest);
    ra_free_temp(high_dest);

    if (opnd0_size == 16) {
        la_append_ir2_opnd2ii(LISA_BSTRINS_D, dest_opnd, dest_opnd, 31, 16);
        la_append_ir2_opnd2ii(LISA_BSTRPICK_D, src_opnd, src_opnd, 31, 16);
        la_append_ir2_opnd2ii(LISA_BSTRINS_D, dest_opnd, src_opnd, 15, 0);
    }
    ra_free_temp(src_opnd);

    IR2_OPND count_opnd = ir2_opnd_new(IR2_OPND_IMM, (int16)count);
    generate_eflag_calculation(final_dest, dest_opnd, count_opnd, pir1, true);

    store_ireg_to_ir1(final_dest, ir1_get_opnd(pir1, 0), false);
    ra_free_temp(final_dest);
    ra_free_temp(dest_opnd);

    return true;
}

bool translate_shld(IR1_INST *pir1)
{
    if (ir1_opnd_is_imm(ir1_get_opnd(pir1, 0) + 2))
        return translate_shld_imm(pir1);
    else
        return translate_shld_cl(pir1);

    return true;
}

bool translate_bswap(IR1_INST *pir1)
{
    IR2_OPND bswap_opnd;
    IR1_OPND *ir1_opnd = ir1_get_opnd(pir1, 0);
    int opnd_size = ir1_opnd_size(ir1_opnd);

    if(opnd_size == 16){
        ir1_opnd->size = (32 >> 3);
        switch (ir1_opnd->reg) {
            case X86_REG_AX:
                ir1_opnd->reg = X86_REG_EAX;
                break;
            case X86_REG_BX:
                ir1_opnd->reg = X86_REG_EBX;
                break;
            case X86_REG_CX:
                ir1_opnd->reg = X86_REG_ECX;
                break;
            case X86_REG_DX:
                ir1_opnd->reg = X86_REG_EDX;
                break;
            case X86_REG_SP:
                ir1_opnd->reg = X86_REG_ESP;
                break;
            case X86_REG_BP:
                ir1_opnd->reg = X86_REG_EBP;
                break;
            case X86_REG_SI:
                ir1_opnd->reg = X86_REG_ESI;
                break;
            case X86_REG_DI:
                ir1_opnd->reg = X86_REG_EDI;
                break;
            default:
                lsassert(0);
                break;
        }
        bswap_opnd =
            load_ireg_from_ir1(ir1_opnd, UNKNOWN_EXTENSION, false);
        la_append_ir2_opnd2ii(LISA_BSTRINS_W, bswap_opnd, zero_ir2_opnd, 15, 0);
    } else if (opnd_size == 32) {
        bswap_opnd = load_ireg_from_ir1(ir1_opnd, UNKNOWN_EXTENSION, false);

        la_append_ir2_opnd2(LISA_REVB_2W, bswap_opnd, bswap_opnd);
    } else {
        lsassert(opnd_size == 64);
        bswap_opnd = load_ireg_from_ir1(ir1_opnd, UNKNOWN_EXTENSION, false);

        la_append_ir2_opnd2(LISA_REVB_D, bswap_opnd, bswap_opnd);
    }
   /*
    * FIXME: high 32bit sign extension may corrupt, add.w 0 to resolve
    */
    ir2_opnd_set_em(&bswap_opnd, UNKNOWN_EXTENSION, 32);

    store_ireg_to_ir1(bswap_opnd, ir1_get_opnd(pir1, 0), false);

    return true;
}
