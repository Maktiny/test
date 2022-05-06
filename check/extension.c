#include<stdio.h>
#include "extension.cpp"

int call_get_id(struct student* p);
int get_id(struct student *p){
  return call_get_id(struct student *p);
}

void set_student_id(struct student *p, int id);
void set_id(struct student *p, int id){
  p->id = id;
}

int main(){
  int *p;
  set_id(p, 11);
  int id = get_id(p);
  printf("studnet is : %d", id);
  return 0;
}
