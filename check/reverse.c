#include<stdio.h>
#include <stdlib.h>
/**/

struct ListNode {
      int val;
      struct ListNode *next;
};


struct ListNode* reverseList(struct ListNode* head){
    struct ListNode* p = head;
    struct ListNode* root = NULL;
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

    struct ListNode* root = (struct ListNode*)malloc(sizeof(struct ListNode));

    struct ListNode * p = root;
    for(int i = 0;i < 4; i++) {
        struct ListNode * node = (struct ListNode *)malloc(sizeof(struct ListNode));
        node->val = i;
        node->next = NULL;
        p->next = node;
        p = p->next;
    }
    struct ListNode* ret = reverseList(root);
    while(ret){
        printf("scs %d\n",ret->val);
        ret = ret->next;
    }

    return 0;

}