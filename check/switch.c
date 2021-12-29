#include<stdio.h>

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

int function(int a,int b){
  printf("result %d\n",a+b);
   return a + b;
}

int main()
{
  int a[] = {1,2,3,4,5,6};
  switch (a[0])
  {
  case 1:
    function(1,1);
    break;
  case 2:
    function(2,2);
    break;
  default:
    break;
  }
  return 0;
}
