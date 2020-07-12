#include <stdio.h>  
#include <sys/types.h>   
#include <sys/ipc.h>   
#include <sys/shm.h>   
#include <signal.h>         
 
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define SIZE 1024    
 
int main()  {      
   int shmid;      
    
   void *shmaddr;      
   struct shmid_ds shm_stat;      
    
   if ( (shmid = shmget((key_t)8282, SIZE, IPC_CREAT|0666)) == -1 )          
      printf("shmget failed"); 

   if ( (shmaddr = shmat(shmid, (void *)0, 0)) == (void*)-1 )          
      printf("shmat failed");      

   for(int i=0; i<10; i++) {
       strcpy((char*)shmaddr, "Prog");
       sleep(1);  
       printf("B : %s\n", (char *)shmaddr);      
   }

   if ( shmctl(shmid, IPC_STAT, &shm_stat) == -1)             
       printf("shmctl failed");      
       
   if ( shmdt(shmaddr) == -1)           
       printf("shmdt failed");      
        
   return 0;
} 