int main(){
    /*at&t: opcode source dest*/
    asm(
        "test $1, %al \n\t"
        "test $1, %ax \n\t"
        "test $1, %eax \n\t"
        "test $1, %rax \n\t"


        "test %al, %bl \n\t"
        "test %ax, %bx \n\t"
        "test %eax, %ebx \n\t"
        "test %rax, %rbx \n\t"

         "test (%rsp), %rbx \n\t"
    );
    return 0;
}