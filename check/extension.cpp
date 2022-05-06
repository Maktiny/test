#include<bits/stdc++.h>

using namespace std;

class student {
  private:
    string name;
    int id;

  public:
    
    void set_student_name(string str){
      name = str;
    }

    void set_student_id(int index){
      id  = index;
    }
    
    int get_student_id(void){
      return id;
    }
};

extern "C" int call_get_id(student * st){
  return st->get_student_id();
}

extern "C" void call_set_id(student *st, int index){
  st->set_student_id(index);
}


