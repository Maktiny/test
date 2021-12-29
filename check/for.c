#include<stdio.h>
// this two types macro, the last line value as return value;
//#define macro_define(a,b) (test(a,b))
#define macro_define(a,b) {(a+b)}

int test(int a, int b){
    return a + b;
}
// #define macro_define(a,b) \
//   do {   \
//       test(a,b); \
//       } while(0)

#define PRINT2(a, b)      \
   do{               \
       printf("print a\n"); \
       printf("print b\n"); \
       test(a,b); \
       printf("%d",test(a,b)); \
     }while(0)

int main(){
    int a = 0;
    int b = 10;
    PRINT2(a,b);
    int c = macro_define(a,b);
    printf("result is %d\n",c);
    return 0;
}