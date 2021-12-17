#include<stdio.h>

int main(){
    long int  a[100];
    int count = 0;
    asm volatile (
        "std \n\t"
        "lea %0, %%rdi \n\t"
        "mov $28, %%rax \n\t"
        "mov $0x2a, %%rcx \n\t"
        "rep stos %%rax,(%%rdi) \n\t"
        :
        :"m"(a[99])
        :"rdi", "rcx", "rax","cc"
    );
    for (int i = 99; i > 57; i--){
        printf("%ld\n", a[i]);
        count++;
    }
    printf("result is %d", count);
    return 0;
}
// int main(){
//     long int  a[100];
//     int count = 0;
//     asm volatile (
//         "lea %0, %%rdi \n\t"
//         "mov $28, %%rax \n\t"
//         "mov $0x2a, %%rcx \n\t"
//         "rep stos %%rax,(%%rdi) \n\t"
//         :
//         :"m"(a)
//         :"rdi", "rcx", "rax","cc"
//     );
//     for (int i = 0; i < 42; i++){
//         printf("%ld\n", a[i]);
//         count++;
//     }
//     printf("result is %d", count);
//     return 0;
// }

// bool translate_stos(IR1_INST *pir1)
// {
//     BITS_SET(pir1->flags, FI_MDA);
//     IR2_OPND edi_opnd = ra_alloc_gpr(edi_index);
//     IR1_PREFIX prefix_flag = ir1_prefix(pir1);
//     int opnd_size = ir1_opnd_size(ir1_get_opnd(pir1, 1));
//     int shift_count = 0;
//     int size = 256 / opnd_size;

//     /*
//      * The rep-loop may not be executed, the em of EDI will be
//      * set to EM_X86_ADDRESS after translation, it may cause a
//      * segfault, so move the `and edi, edi, n1` out of the loop.
//      */
// #ifndef TARGET_X86_64
//     if (ir2_opnd_em(&edi_opnd) == SIGN_EXTENSION) {
//         la_append_ir2_opnd2_em(LISA_CLR_H32, edi_opnd, edi_opnd);
//         ir2_opnd_set_em(&edi_opnd, EM_X86_ADDRESS, 32);
//     }
// #endif

//     /* 1. exit when initial count is zero */
//     IR2_OPND label_exit = ir2_opnd_new_type(IR2_OPND_LABEL);
//     IR2_OPND ecx_opnd;
//     if (prefix_flag != 0) {
// #ifndef TARGET_X86_64
//         ecx_opnd = load_ireg_from_ir1(&ecx_ir1_opnd, SIGN_EXTENSION, false);
// #else
//         ecx_opnd = load_ireg_from_ir1(&rcx_ir1_opnd, SIGN_EXTENSION, false);
// #endif
//         la_append_ir2_opnd3(LISA_BEQ, ecx_opnd, zero_ir2_opnd, label_exit);
//     }

//     /* 2. preparations outside the loop */
//     IR2_OPND eax_value_opnd =
//         load_ireg_from_ir1(ir1_get_opnd(pir1, 1), SIGN_EXTENSION, false);
//     IR2_OPND step_opnd = ra_alloc_itemp();
//     load_step_to_reg(&step_opnd, pir1);
//     IR2_OPND label_loop_begin = ir2_opnd_new_type(IR2_OPND_LABEL);
//     IR2_OPND label_df0 = ir2_opnd_new_type(IR2_OPND_LABEL);
//     IR2_OPND label_df1 = ir2_opnd_new_type(IR2_OPND_LABEL);
//     /*@1 preparations the Vectorization*/
//     if (prefix_flag != 0) {
//         IR2_OPND vtemp = ra_alloc_ftemp();
//         IR2_OPND vloop_count = ra_alloc_itemp();
//         IR2_OPND step_count = ra_alloc_itemp();
//         IR2_OPND label_vloop_begin = ir2_opnd_new_type(IR2_OPND_LABEL);
//         /*use al/ax/eax/rax to fill the 256bits vtemp */
//         switch (opnd_size) {
//         case 8:
//             la_append_ir2_opnd2i(LISA_VINSGR2VR_B, vtemp, eax_value_opnd, 0);
//             la_append_ir2_opnd2(LISA_XVREPLVE0_B, vtemp, vtemp);
//             shift_count = 5;
//             break;
//         case 16:
//             la_append_ir2_opnd2i(LISA_VINSGR2VR_H, vtemp, eax_value_opnd, 0);
//             la_append_ir2_opnd2(LISA_XVREPLVE0_H, vtemp, vtemp);
//             shift_count = 4;
//             break;
//         case 32:
//             la_append_ir2_opnd2i(LISA_VINSGR2VR_W, vtemp, eax_value_opnd, 0);
//             la_append_ir2_opnd2(LISA_XVREPLVE0_W, vtemp, vtemp);
//             shift_count = 3;
//             break;
//         case 64:
//             la_append_ir2_opnd2i(LISA_VINSGR2VR_D, vtemp, eax_value_opnd, 0);
//             la_append_ir2_opnd2(LISA_XVREPLVE0_D, vtemp, vtemp);
//             shift_count = 2;
//             break;
//         }
//         la_append_ir2_opnd1(LISA_LABEL, label_vloop_begin);
//         /* ecx / size*/
// #ifndef TARGET_X86_64
//         la_append_ir2_opnd2i_em(LISA_SRLI_W, vloop_count, ecx_opnd, shift_count);
// #else
//         la_append_ir2_opnd2i_em(LISA_SRLI_D, vloop_count, ecx_opnd, shift_count);
// #endif
//         /*when the loop times (ecx) too small to don't fill 256 bits, jump to
//          * slow path */
//         la_append_ir2_opnd3(LISA_BEQ, vloop_count, zero_ir2_opnd,
//                             label_loop_begin);
//          la_append_ir2_opnd3(LISA_BLT, step_opnd, zero_ir2_opnd,
//                             label_df0);
//         la_append_ir2_opnd3(LISA_BGE, step_opnd, zero_ir2_opnd,
//                             label_df1);
//         /*DF = 0, EDI is increased*/
//         /*write the vtemp to [EDI]*/
//         la_append_ir2_opnd1(LISA_LABEL, label_df0);

//         la_append_ir2_opnd2i(LISA_XVST, vtemp, edi_opnd, 0);
//         /* adjust the EDI*/
// #ifndef TARGET_X86_64
//         la_append_ir2_opnd2i_em(LISA_SLLI_W, step_count, step_opnd, shift_count);
// #else
//         la_append_ir2_opnd2i_em(LISA_SLLI_D, step_count, step_opnd, shift_count);
// #endif
//         la_append_ir2_opnd3_em(LISA_SUB_ADDRX, edi_opnd, edi_opnd, step_count);

//         la_append_ir2_opnd1(LISA_LABEL, label_df1);
//         /*DF = 1, EDI is decreased*/
//         int offset = -32 + opnd_size / 8;
// #ifndef TARGET_X86_64
//         la_append_ir2_opnd2i_em(LISA_ADDI_W, edi_opnd, edi_opnd, offset);
// #else
//         la_append_ir2_opnd2i_em(LISA_ADDI_D, edi_opnd, edi_opnd, offset);
// #endif
//         la_append_ir2_opnd2i(LISA_XVST, vtemp, edi_opnd, 0);
//         la_append_ir2_opnd3_em(LISA_SUB_ADDRX, edi_opnd, edi_opnd, step_opnd);


//         /*ecx = ecx - size*/
// #ifndef TARGET_X86_64
//         la_append_ir2_opnd2i_em(LISA_ADDI_W, ecx_opnd, ecx_opnd, -size);
// #else
//         la_append_ir2_opnd2i_em(LISA_ADDI_D, ecx_opnd, ecx_opnd, -size);
// #endif
//         /*if ecx != 0, use fast path*/
//         la_append_ir2_opnd3(LISA_BNE, ecx_opnd, zero_ir2_opnd,
//                             label_vloop_begin);
//         /*if ecx == 0, jump to end*/
//         la_append_ir2_opnd3(LISA_BEQ, ecx_opnd, zero_ir2_opnd, label_exit);
//         ra_free_temp(vtemp);
//         ra_free_temp(step_count);
//         ra_free_temp(vloop_count);
//     }

//     /* 3. loop starts */

//     la_append_ir2_opnd1(LISA_LABEL, label_loop_begin);

//     /* 3.1 store EAX into memory at EDI */
//     store_ireg_to_ir1(eax_value_opnd, ir1_get_opnd(pir1, 0), false);

//     /* 3.2 adjust EDI */
//     la_append_ir2_opnd3_em(LISA_SUB_ADDRX, edi_opnd, edi_opnd, step_opnd);

//     /* 4. loop ends? when ecx==0 */
//     if (prefix_flag != 0) {
//         lsassert(ir1_prefix(pir1) == X86_PREFIX_REP);
// #ifndef TARGET_X86_64
//         la_append_ir2_opnd2i_em(LISA_ADDI_W, ecx_opnd, ecx_opnd, -1);
// #else
//         la_append_ir2_opnd2i_em(LISA_ADDI_D, ecx_opnd, ecx_opnd, -1);
// #endif
//         la_append_ir2_opnd3(LISA_BNE, ecx_opnd, zero_ir2_opnd,
//                             label_loop_begin);
//     }

//     /* 5. exit */
//     la_append_ir2_opnd1(LISA_LABEL, label_exit);
//     if (prefix_flag != 0) {
//         store_ireg_to_ir1(ecx_opnd, &ecx_ir1_opnd, false);
//     }

//     ra_free_temp(step_opnd);

//     return true;
// }


bool translate_stos(IR1_INST *pir1)
{
    BITS_SET(pir1->flags, FI_MDA);
    IR2_OPND edi_opnd = ra_alloc_gpr(edi_index);
    IR1_PREFIX prefix_flag = ir1_prefix(pir1);
    int opnd_size = ir1_opnd_size(ir1_get_opnd(pir1, 1));
    int shift_count = 0;
    int size = 256 / opnd_size;

    /*
     * The rep-loop may not be executed, the em of EDI will be
     * set to EM_X86_ADDRESS after translation, it may cause a
     * segfault, so move the `and edi, edi, n1` out of the loop.
     */
#ifndef TARGET_X86_64
    if (ir2_opnd_em(&edi_opnd) == SIGN_EXTENSION) {
        la_append_ir2_opnd2_em(LISA_CLR_H32, edi_opnd, edi_opnd);
        ir2_opnd_set_em(&edi_opnd, EM_X86_ADDRESS, 32);
    }
#endif

    /* 1. exit when initial count is zero */
    IR2_OPND label_exit = ir2_opnd_new_type(IR2_OPND_LABEL);
    IR2_OPND ecx_opnd;
    if (prefix_flag != 0) {
#ifndef TARGET_X86_64
        ecx_opnd = load_ireg_from_ir1(&ecx_ir1_opnd, SIGN_EXTENSION, false);
#else
        ecx_opnd = load_ireg_from_ir1(&rcx_ir1_opnd, SIGN_EXTENSION, false);
#endif
        la_append_ir2_opnd3(LISA_BEQ, ecx_opnd, zero_ir2_opnd, label_exit);
    }

    /* 2. preparations outside the loop */
    IR2_OPND eax_value_opnd =
        load_ireg_from_ir1(ir1_get_opnd(pir1, 1), SIGN_EXTENSION, false);
    IR2_OPND step_opnd = ra_alloc_itemp();
    load_step_to_reg(&step_opnd, pir1);
    IR2_OPND label_loop_begin = ir2_opnd_new_type(IR2_OPND_LABEL);
    IR2_OPND label_df0 = ir2_opnd_new_type(IR2_OPND_LABEL);
    IR2_OPND label_df1 = ir2_opnd_new_type(IR2_OPND_LABEL);
    /*@1 preparations the Vectorization*/
    if (prefix_flag != 0) {
        IR2_OPND vtemp = ra_alloc_ftemp();
        IR2_OPND vloop_count = ra_alloc_itemp();
        IR2_OPND step_count = ra_alloc_itemp();
        IR2_OPND label_vloop_begin = ir2_opnd_new_type(IR2_OPND_LABEL);
        /*use al/ax/eax/rax to fill the 256bits vtemp */
        switch (opnd_size) {
        case 8:
            la_append_ir2_opnd2i(LISA_VINSGR2VR_B, vtemp, eax_value_opnd, 0);
            la_append_ir2_opnd2(LISA_XVREPLVE0_B, vtemp, vtemp);
            shift_count = 5;
            break;
        case 16:
            la_append_ir2_opnd2i(LISA_VINSGR2VR_H, vtemp, eax_value_opnd, 0);
            la_append_ir2_opnd2(LISA_XVREPLVE0_H, vtemp, vtemp);
            shift_count = 4;
            break;
        case 32:
            la_append_ir2_opnd2i(LISA_VINSGR2VR_W, vtemp, eax_value_opnd, 0);
            la_append_ir2_opnd2(LISA_XVREPLVE0_W, vtemp, vtemp);
            shift_count = 3;
            break;
        case 64:
            la_append_ir2_opnd2i(LISA_VINSGR2VR_D, vtemp, eax_value_opnd, 0);
            la_append_ir2_opnd2(LISA_XVREPLVE0_D, vtemp, vtemp);
            shift_count = 2;
            break;
        }
        la_append_ir2_opnd1(LISA_LABEL, label_vloop_begin);
        /* ecx / size*/
#ifndef TARGET_X86_64
        la_append_ir2_opnd2i_em(LISA_SRLI_W, vloop_count, ecx_opnd, shift_count);
#else
        la_append_ir2_opnd2i_em(LISA_SRLI_D, vloop_count, ecx_opnd, shift_count);
#endif
        /*when the loop times (ecx) too small to don't fill 256 bits, jump to
         * slow path */
        la_append_ir2_opnd3(LISA_BEQ, vloop_count, zero_ir2_opnd,
                            label_loop_begin);

        la_append_ir2_opnd3(LISA_BGE, step_opnd, zero_ir2_opnd,
                            label_df1);

        /*DF = 0, EDI is increased*/
        /*write the vtemp to [EDI]*/
        la_append_ir2_opnd2i(LISA_XVST, vtemp, edi_opnd, 0);
        /* adjust the EDI*/
#ifndef TARGET_X86_64
        la_append_ir2_opnd2i_em(LISA_SLLI_W, step_count, step_opnd, shift_count);
#else
        la_append_ir2_opnd2i_em(LISA_SLLI_D, step_count, step_opnd, shift_count);
#endif
        la_append_ir2_opnd3_em(LISA_SUB_ADDRX, edi_opnd, edi_opnd, step_count);

        la_append_ir2_opnd3(LISA_BLT, step_opnd, zero_ir2_opnd,
                            label_df0);

        la_append_ir2_opnd1(LISA_LABEL, label_df1);
        /*DF = 1, EDI is decreased*/
        int offset = -32 + opnd_size / 8;
#ifndef TARGET_X86_64
        la_append_ir2_opnd2i_em(LISA_ADDI_W, edi_opnd, edi_opnd, offset);
#else
        la_append_ir2_opnd2i_em(LISA_ADDI_D, edi_opnd, edi_opnd, offset);
#endif
        la_append_ir2_opnd2i(LISA_XVST, vtemp, edi_opnd, 0);
        la_append_ir2_opnd3_em(LISA_SUB_ADDRX, edi_opnd, edi_opnd, step_opnd);


        la_append_ir2_opnd1(LISA_LABEL, label_df0);
        /*ecx = ecx - size*/
#ifndef TARGET_X86_64
        la_append_ir2_opnd2i_em(LISA_ADDI_W, ecx_opnd, ecx_opnd, -size);
#else
        la_append_ir2_opnd2i_em(LISA_ADDI_D, ecx_opnd, ecx_opnd, -size);
#endif
        /*if ecx != 0, use fast path*/
        la_append_ir2_opnd3(LISA_BNE, ecx_opnd, zero_ir2_opnd,
                            label_vloop_begin);
        /*if ecx == 0, jump to end*/
        la_append_ir2_opnd3(LISA_BEQ, ecx_opnd, zero_ir2_opnd, label_exit);
        ra_free_temp(vtemp);
        ra_free_temp(step_count);
        ra_free_temp(vloop_count);
    }

    /* 3. loop starts */

    la_append_ir2_opnd1(LISA_LABEL, label_loop_begin);

    /* 3.1 store EAX into memory at EDI */
    store_ireg_to_ir1(eax_value_opnd, ir1_get_opnd(pir1, 0), false);

    /* 3.2 adjust EDI */
    la_append_ir2_opnd3_em(LISA_SUB_ADDRX, edi_opnd, edi_opnd, step_opnd);

    /* 4. loop ends? when ecx==0 */
    if (prefix_flag != 0) {
        lsassert(ir1_prefix(pir1) == X86_PREFIX_REP);
#ifndef TARGET_X86_64
        la_append_ir2_opnd2i_em(LISA_ADDI_W, ecx_opnd, ecx_opnd, -1);
#else
        la_append_ir2_opnd2i_em(LISA_ADDI_D, ecx_opnd, ecx_opnd, -1);
#endif
        la_append_ir2_opnd3(LISA_BNE, ecx_opnd, zero_ir2_opnd,
                            label_loop_begin);
    }

    /* 5. exit */
    la_append_ir2_opnd1(LISA_LABEL, label_exit);
    if (prefix_flag != 0) {
        store_ireg_to_ir1(ecx_opnd, &ecx_ir1_opnd, false);
    }

    ra_free_temp(step_opnd);

    return true;
}
