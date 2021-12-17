#include <stdio.h>
#include <inttypes.h>
#include <stdint.h>

char a = 'a';
int main(){
    char b = 'z';
    asm volatile (
        "lea %1, %%rsi \r\n"
        "mov $'b', %%rax \r\n"
        "mov $0, %%rcx \r\n"
        "rep lodsb\r\n"
        "mov %%al, %0 \r\n"
        :"=m"(b)
        :"m"(a)
        :"rax", "rcx", "rsi"
    );

    printf("%x\n", b);
    return 0;
}