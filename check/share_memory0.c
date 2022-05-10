#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>

#define SHM_PATH "/tmp/shm"
#define SHM_SIZE 128

int main(int argc, char* argv[]){
  int shmid;
  char* addr;
 /*系统建立IPC通讯（如消息队列、共享内存时）必须指定一个ID值 。通常情况下，该id值通过ftok函数得到 。  
   key_t ftok( char * fname, int id )
   参数说明：
        fname就时您指定的文档名
        id是子序号。
  */
  key_t key = ftok(SHM_PATH, 0x6666);
  
  /*
   * 得到一个共享内存标识符或创建一个共享内存对象并返回共享内存标识符
   *int shmget( key_t, size_t, flag);
   * */
  shmid = shmget(key, SHM_SIZE, IPC_CREAT|IPC_EXCL|0666);
  if(shmid < 0){
    printf("failed to create a share memory object\n");
    return -1;
  }

  /**
   *shmat（）是用来允许本进程访问一块共享内存的函数，与shmget（）函数共同使用。
   *shmat的原型是：void *shmat（int shmid，const void *shmaddr,int shmflg）;
   * 如果 shmaddr 是NULL，系统将自动选择一个合适的地址！ 如果shmaddr不是NULL 并且没有指定SHM_RND 则此段连接到addr所指定的地址上 
   * shmat返回值是该段所连接的实际地址 如果出错返回-1
   * */
   addr = shmat(shmid, NULL, 0);
   if(addr <= 0){
     printf("failed to map share memory\n");
     return -1;
   }
   /*向addr指定的地址写入内容(字符)，不包括字符串结束符*/
   sprintf(addr, "%s", "hello world!\n");

   return 0;
}
