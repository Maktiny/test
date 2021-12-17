int main(){
    /*at&t: opcode source dest*/
    asm(
        "sar $0, %bh \n\t"
        "sar $1, %ax \n\t"
        "sar $1, %ebx \n\t"
        "sar $1, %rax \n\t"

        "sar %cl, %bh \n\t"
        "sar %cl, %dx \n\t"
        "sar %cl, %ebx \n\t"
        "sar %cl, %rax \n\t"

        "sar $0xff, %rax \n\t"
        "sar $0x11, %rax \n\t"

        "sar $0xff, %bh \n\t"
        "sar $0xff, %bx \n\t"
        "sar $0x01, %ebx \n\t"
        "sar $0x3f, %rbx \n\t"

    );
    return 0;
}
/*

*/