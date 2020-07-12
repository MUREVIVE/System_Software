#include <unistd.h>
#include <signal.h>
//#include <siginfo.h>
#include <stdlib.h>
#include <stdio.h>


void sig_handler(int signo) {
    psignal(signo, "Recived Signal:");
    sleep(2);
    printf("In Signal Handler, After Sleep\n");
}

int main(void) {
    struct sigaction act; // sig에 지정한 시그널을 받을 때 처리하는 방법을 act로 받음

    sigemptyset(&act.sa_mask); // act.sa_mask의 주소로 지정한 시그널 집합 비우기
    sigaddset(&act.sa_mask, SIGQUIT); // signal handler가 동작하는 중에 SIGQUIT 시그널 블록 위해

    act.sa_handler = sig_handler; // sig_handler의 함수 포인터를 sa_handler가 받음

    if(sigaction(SIGQUIT, &act, (struct sigaction *)NULL) < 0) {
        perror("sigaction");
        exit(1);
    }

    fprintf(stderr, "Input SIGQUIT: ");
    pause();
    fprintf(stderr, "After Signal Handler\n");
    fprintf(stderr, "program end");
    return 0;
}