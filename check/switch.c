#include<stdio.h>
#include<stdint.h>

// int main(){

//   int a[] = {1,2,4,5,6,50,60,70,80,90};
//   for(int i =0; i < 10; i++){
//     int b = a[i];
//     switch(b){
//       case 1 ... 10:
//         printf("b is %d\n", b); break;//表示范围
//     }
//   }

//   return 0;
// }

typedef struct CALL_RETURN {
    uint64_t SPC;
    uint64_t TPC;
} CALL_RETURN;

typedef struct cbc
{
  CALL_RETURN *call;
} cbc;



int main()
{

  int e = 0;
  cbc b;

  struct CALL_RETURN c ;
  c.SPC = 2;
  b.call = &c;



  printf("%ld\n",b.call->SPC);

  return 0;
}
