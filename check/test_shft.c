#include<stdio.h>

int main()
{
  unsigned int a = 0xff10f;
  unsigned int b = 1 << 8;
  unsigned int c = a & b;
  printf("result is 0x%16x\n",a);
  printf("result is 0x%16x\n",b);
  printf("result is 0x%16x\n",c);

}
