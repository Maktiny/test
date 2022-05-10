#include<stdio.h>
#include<signal.h>
#include<unistd.h>

void sigcb(int sign){
  switch(sign) {
    case SIGHUP:
      printf("get a signal ----SIGHUP\n");
      break;
    case SIGINT:
      printf("get a signal --SIGINT\n");
      break;
    case SIGQUIT:
      printf("get a signal -- SIGQUIT\n");
      break;
  }
  return;
}

int main(){
  signal(SIGHUP,sigcb);
  signal(SIGINT,sigcb);
  signal(SIGQUIT,sigcb);
  
  while(1){
    sleep(10);
  }

  return 0;
}
