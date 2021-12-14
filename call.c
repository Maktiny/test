#include<stdio.h>
int main()
{
  asm(
    "call 1f \n\t"
    "1: nop \n\t"
  );
  return 0;
}
