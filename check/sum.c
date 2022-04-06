#include<stdio.h>
int add(int a, int b) {
        if (b == 0) {
            return a;
        }
        printf("number\n");
        // 转换成非进位和 + 进位
        return add(a ^ b, (a & b) << 1);
    }
int main(){
  int a = 4;
  int b = 5;
  int c = a&b;
  int d = a^b;
  printf("result is %d\n", add(a,b));
  printf("result is %d\n", c|d);
}
