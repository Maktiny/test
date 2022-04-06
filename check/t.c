#include<stdio.h>

#include <stdlib.h>


void quickSort2(int* nums, int l, int r){
    int i = l;
    int j = r;
    if(l < r){
        while(l < r){
            while(l < r && nums[r] >= nums[l])
                  r--;
            int temp = nums[l];
            nums[l] = nums[r];
            nums[r] = temp;

            while(l < r && nums[l] <= nums[r])
                  l++;
           temp = nums[l];
            nums[l] = nums[r];
            nums[r] = temp;
        }
        quickSort2(nums, i, l - 1);//递归左边，此时l=5
        quickSort2(nums, l + 1, j);//递归右边，此时l=5
    }

}

void sort(int* arr, int l, int r) {
    int i = l;
    int j = r;
    if (l < r) {
        while (l < r) {
            while (l < r && arr[r] >= arr[l]) {
                r--;
            }
            int tmp = arr[l];
            arr[l] = arr[r];
            arr[r] = tmp;

            while (l < r && arr[l] <= arr[r]) {
                l++;
            }
            tmp = arr[l];
            arr[l] = arr[r];
            arr[r] = tmp;

        }
        sort(arr, i, l - 1);//递归左边，此时l=5
        sort(arr, l + 1, j);//递归右边，此时l=5
    }
}

int search(int* nums,  int numsSize, int target){
    int left = 0;
    int right = numsSize - 1;
    int index = 0, count = 0;

    while(left < right){
        int mid = left + (right - left) / 2;
        if(target == nums[mid]){
            index = mid;
            break;
        }
        else if(target > nums[mid])
             left = mid + 1;
        else if (target < nums[mid])
              right = mid - 1;

    }

    return index;
}

int search1(int* nums, int numsSize, int target){
    int left = 0;
    int right = numsSize - 1;
    int lindex = 0,rindex = 0, count = 0;

    while(left < right){ //右边界
        int mid = left + (right - left) / 2;
        if(nums[mid] <= target)
             left = mid + 1;
        else
              right = mid - 1;
    }

    rindex = left ;
    printf("result is %d\n",rindex );


    while(left < right){
        int mid = left + (right - left) / 2;
        if(nums[mid] <= target ){
            right = mid -1;
        }
        else
             left = mid + 1;
    }
    lindex = right;
     printf("result is %d\n",lindex );
    return rindex - lindex -1;
}


int coinChange(int* coins, int coinsSize, int amount){
    int *dp = (int *)malloc(amount * sizeof(int));

    for(int i = 1;i < amount; i++) {
        int max = -10000;
        dp[i] = -1;
        for(int j = 0; j < coinsSize; j++){
            if(((i - coins[j]) < 0){
                dp[i] = dp[i - 1];
            }
            else{
                if(coins[j] > max)
                     max = coins[j];
            }
        }
        if(max > 0)
           dp[i] = dp[i - 1] +1;
    }
    return dp[amount];
}

int** eraseOverlapIntervals(int** intervals, int intervalsSize){
    int* temp;
    for(int i = 0; i < intervalsSize; i ++){
        for(int j = 0; j < intervalsSize; j++){
               if(intervals[i][1] < intervals[j][1]){
                   temp = intervals[i];
                   intervals[i] = intervals[j];
                   intervals[j] = temp;
               }
        }
    }
    return intervals;
}

int main(){
   int a = {{1,2},{2,3},{3,4},{1,3}};

   int ** b= eraseOverlapIntervals(a,4);
for(int i = 0; i < 4; i ++){
        for(int j = 0; j < 4; j++){
               printf("%d ", b[i][j]);
        }
    }
   //quickSort2(ret,0,10);
   //int r = search1(ret,6,8);




    return 0;
}


