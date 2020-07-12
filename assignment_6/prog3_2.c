#include <stdio.h> 
#include <errno.h> 
#include <sys/types.h> 
#include <sys/ipc.h> 
#include <sys/sem.h>
#include <sys/shm.h> 
#include <signal.h>

#include <unistd.h> 
#include <stdlib.h> 
#include <string.h>

#define SIZE 1024

// 세마포어 공용체 정의
union semun { 
    int val; 
    struct semid_ds *buf; 
    unsigned short *array; 
}; 

int semCreate(key_t semkey) { 
    union semun semunarg; 
    int status = 0, semid; 
    
    
    semid = semget(semkey, 1, IPC_CREAT | IPC_EXCL | 0600); 
    
    if(semid == -1) { 
        if (errno == EEXIST) 
            semid = semget(semkey, 1, 0); 
    } 
    else { 
        semunarg.val = 1; 
        status = semctl(semid, 0, SETVAL, semunarg); 
    } 
    
    if(semid == -1 || status == -1) { 
        perror("semCreate"); 
        return (-1); 
    } 
    return semid; 
} 

int semlock(int semid) { 
    
    struct sembuf buf; 
    buf.sem_num = 0; 
    buf.sem_op = -1; 
    buf.sem_flg = SEM_UNDO; 
    
    if(semop(semid, &buf, 1) == -1) { 
        perror("semlock failed"); 
        exit(1); 
    } 
    
    return 0; 
} 

int semunlock(int semid) { 
    
    struct sembuf buf; 
    buf.sem_num = 0; 
    buf.sem_op = 1; 
    buf.sem_flg = SEM_UNDO; 
    
    if(semop(semid, &buf, 1) == -1) { 
        perror("semunlock failed"); 
        exit(1); 
    } 
    
    return 0; 
} 

int shmid;
void *shmaddr;      
struct shmid_ds shm_stat;
// semProcess 함수는 세마포어를 생성해 잠금을 수행하고 
// 작업한 후 다시 잠금을 해제한다. 
void semProcess() { 
    int semid; 
    
    pid_t pid = getpid(); 
    
    if((semid = semCreate(1)) < 0) 
        exit(1); 
    semlock(semid); 
    
    strcpy((char*)shmaddr, "Prog");
    sleep(1); 
    printf("\nB : %s", (char *)shmaddr);
    
    semunlock(semid); 
    exit(0); 
} 

int main(void) {       
    
    if ( (shmid = shmget((key_t)8282, SIZE, IPC_CREAT|0666)) == -1 )          
      printf("shmget failed"); 

    if ( (shmaddr = shmat(shmid, (void *)0, 0)) == (void*)-1 )            
        printf("shmat failed");      

    for(int i = 0; i < 10; i++) 
        if(fork() == 0) semProcess(); 

   if ( shmctl(shmid, IPC_STAT, &shm_stat) == -1)             
       printf("shmctl failed");      
       
   if ( shmdt(shmaddr) == -1)           
       printf("shmdt failed");    
    
    return 0; 
}

