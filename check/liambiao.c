#include<stdio.h>
#include <stdlib.h>
/**/

typedef struct ListNode {
      int val;
      struct ListNode *next;
}ListNode;


int* reversePrint(ListNode* head, int* returnSize){
     ListNode* p = head->next;
    int count = 0;
    while(p){
        count++;
        p = p->next;
    }
    printf("result is %d\n", count);
    *returnSize = count;
    int* ret = (int *)malloc(count * sizeof(int));

    int index = count - 1;
    p = head->next;
    while(p){
        ret[index] = p->val;
        printf("result is %d\n", p->val);
        index--;
        p = p->next;
    }
    return ret;
}

struct ListNode* reverseList(struct ListNode* head){
    struct ListNode* p = (struct ListNode*)malloc(sizeof(struct ListNode));
    struct ListNode* root = (struct ListNode*)malloc(sizeof(struct ListNode));
    p = head;
    while(head) {
        p = head->next;
        head->next = root;
        root = head;
        head = p;
    }
    return root;
}

int main(){

    ListNode * root = (ListNode *)malloc(sizeof(ListNode));

    ListNode * p = root;
    for(int i = 0;i < 4; i++) {
        ListNode * node = (ListNode *)malloc(sizeof(ListNode));
        node->val = i;
        node->next = NULL;
        p->next = node;
        p = p->next;
    }
    int length = 3;
    int * ret = reversePrint(root,&length);
    ListNode* * ret = reverseList(root);
    while(ret){
        printf("scs %d\n",ret->val);
        ret = ret->next;
    }

    return 0;

}