#include<stdio.h>
#include<stdlib.h>
#include<string.h>
int candy(int* ratings, int ratingsSize){
    int candy[ratingsSize];
    memset(candy, 1, sizeof(candy));
    for(int i = 0; i < ratingsSize; i++){
        //sum += candy[i];
        printf("candy is %d\n",candy[i]);
    }


    for(int i = 1; i < ratingsSize; i++){
        if(ratings[i] > ratings[i - 1]){
            candy[i] = candy[i - 1] + 1;
        }
    }
    for(int i = ratingsSize -2; i >= 0; i--){
        if(ratings[i] > ratings[i + 1]){
            candy[i] = (candy[i + 1] + 1) > candy[i] ? (candy[i + 1] + 1) : candy[i];
        }
    }
    int sum = 0;
    for(int i = 0; i < ratingsSize; i++){
        sum += candy[i];
        printf("candy is %d\n",candy[i]);
    }

    return sum;
}

int main(){
  int b[3] = {0};
  memset(b,2,sizeof(b));
for(int i = 0; i < 3; i++){
        //sum += candy[i];
        printf("b is %d\n",b[i]);
    }


  int a[3] ={1,0,2};
  printf("result is %d\n",candy(a,3));
}
