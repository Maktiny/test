int main(){
    /*at&t: opcode source dest*/
    asm(
        "and 0xf, %rax \n\t"
        "and 0xf, %ebx \n\t"
        "and %eax, %ebx \n\t"
        "and 0xf, %ax \n\t"
        "and 0xffff, %ebx \n\t"
    );
    return 0;
}
/*

*/