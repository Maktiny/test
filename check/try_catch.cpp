#include<iostream>

using namespace std;

double division(int a, int b){
  if(b == 0){
    throw "division by zero!";
  }
  return a/b;
}

int main(){
  int a = 10;
  int b = 0;
  double z = 0.0;

  try{
    z = division(a,b);
    cout<< z <<endl;
  }catch(const char* msg){
    cerr<<msg<<endl;
  }
  return 0;
}
