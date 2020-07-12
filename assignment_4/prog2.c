#include <unistd.h>
#include <signal.h>
#include <stdio.h>

int main(void) {
    sigset_t new;

    sigemptyset(&new);
    sigaddset(&new, SIGINT); // 시그널 집합에 SIGINt 시그널 설정
    // 블록된 시그널을 해제하면 기다리고 있던 SIGQUIT 시그널을 받은 프로세스 
    // 코어 덤프 생성 후 종료
    sigprocmask(SIG_BLOCK, &new, (sigset_t *)NULL); 

    while(1) {
        sleep(1);
    }

    return 0;
}