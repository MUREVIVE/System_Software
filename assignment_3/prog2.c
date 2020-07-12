#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    int status;
    pid_t pid;

    pid = fork();

    if(pid < 0) {
        perror("fork");
        exit(1);
    }
    else if(pid == 0) {
        printf("Child Process : Begin\n");
        system("w");
        //system("who"); 
        printf("Child Process : End\n");
    } 
    else {
        printf("Parent process : Begin\n");
        system("date");
        // 자식 프로세스가 종료하기를 기다린다.
        while(waitpid(pid, &status, WNOHANG) == 0);
        printf("Parent process : End\n");
    }

    return 0;
}