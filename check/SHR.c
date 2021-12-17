int main(){
    /*at&t: opcode source dest*/
    asm(
        "shr $0, %bh \n\t"
        "shr $1, %ax \n\t"
        "shr $1, %ebx \n\t"
        "shr $1, %rax \n\t"

        "shr %cl, %bh \n\t"
        "shr %cl, %dx \n\t"
        "shr %cl, %ebx \n\t"
        "shr %cl, %rax \n\t"

        "shr $0xff, %bh \n\t"
        "shr $0x11, %bx \n\t"
        "shr $0x01, %ebx \n\t"
        "shr $0x25, %rbx \n\t"
    );
    return 0;
}