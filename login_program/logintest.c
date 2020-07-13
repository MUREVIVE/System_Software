#include <time.h>
#include <signal.h>
#include <setjmp.h>
#include <ncurses.h> // curses의 새로운 버전 / 하드웨어에 의존하지 않고 화면 조작
#include <syslog.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>
#include <shadow.h>
#include <crypt.h>

#include <sys/wait.h>

#define KEY_CR_ 0x0d
#define KEY_BACKSPACE_ 0x7f
#define BUFLEN_MAX 255
#define TRY_MAX 5 // login try numbers
#define INIT_X (0)
#define INIT_Y (0)
#define ID_X (INIT_X +10)
#define ID_Y (INIT_Y +4)
#define PASSWORD_X (ID_X)
#define PASSWORD_Y (ID_Y+1)
#define RESULT_X (0)
#define RESULT_Y (PASSWORD_Y+2)

#define EXPIRE_TIME 30 // 30초 이내에 입력하지 않을 시 time expired.
#define BASIC_TIME 5
#define PRINTLINE_X (0)
#define PRINTLINE_Y (PASSWORD_Y+1)

int g_remain;
int g_timer_param;
int line;

//handler for signal of child process
void sig_handler(int signum){

	if(signum != SIGALRM) exit(0);

	//when time (30s) is expired -> 다음 로그인 시도를 10분동안 중지
	if(g_timer_param == EXPIRE_TIME){
		endwin(); // 화면 복구
		clear();
		//move(INIT_Y, INIT_X);
		printf("Time expired (%d seconds)\n",EXPIRE_TIME); // 버퍼에 문자열 출력

        sleep(600); 
		refresh(); // 버퍼 화면 출력
		//getch();
		endwin(); // 화면 복구
		exit(0);
	}//when there is no input for BASIC_TIME(5s)
	else if(g_timer_param == BASIC_TIME){
		g_timer_param = EXPIRE_TIME;
		g_remain -=BASIC_TIME;
		alarm(g_remain);
	}
	else{
		clear();
		move(0,0); // (0,0)으로 커서 이동
		printw("illegal parameter");
		refresh();
		getch();
		exit(0);
	}
}

//input interface
void draw_interface(int nTry){
	clear();
	move(INIT_Y, INIT_X);
	printw(  "###########################################");
	printw("\n# Login program using signal    #");
	printw("\n###########################################");
	printw("\nRemain Time: %d",TRY_MAX-nTry);
	printw("\nLogin ID: ");
	printw("\nPassword: ");
	refresh();
}

#define MODE_PASSWORD 1
#define MODE_ID 2

//get string input from user
int get_string(int mode, char *buf){
	int n, res;
	char ch;

	if(mode !=MODE_PASSWORD && mode != MODE_ID) return -1;

	memset(buf, 0, BUFLEN_MAX);
	noecho();
	
	mode == MODE_PASSWORD? move(PASSWORD_Y, PASSWORD_X) : move(ID_Y, ID_X);
	refresh();
	n=0;

	while((res=read(0,&ch,1))){
	 	if(res<0) return -1;
		
		// Enter입력한 경우
		if(ch==KEY_CR_) break;
		
		// No left time
		if(g_remain < BASIC_TIME){
			g_timer_param = EXPIRE_TIME;
			alarm(g_timer_param);
			g_remain=0;

		}else{//BASIC_TIME 안에 다음 입력히 수행되지 않은 경우
			g_timer_param=BASIC_TIME;
			alarm(g_timer_param);
		}
		
		// backspace 입력한 경우 
		if(ch == KEY_BACKSPACE_){
			if(n>0){
				n--;
				mode == MODE_PASSWORD?
				move(PASSWORD_Y, PASSWORD_X+n) : move(ID_Y,ID_X+n);
		
				printw(" ");
				mode == MODE_PASSWORD?
				move(PASSWORD_Y, PASSWORD_X+n) : move(ID_Y, ID_X+n);
				refresh();
			}
		}// password 처리
		else{
			mode == MODE_PASSWORD?printw("*"):printw("%c",ch);
			refresh();
			buf[n++]=ch;
			if(n>=BUFLEN_MAX) return -1;
		}
	 }
	 buf[n]='\0';
	 return n;
}

//parent's signal handler
void p_handler(int signum){

	//login success
	if(signum == SIGUSR1){
		printf("Login Success\n");
		
	}
	//login fail
	if(signum == SIGUSR2) {
		printf("Login Fail\n");
		sigset_t new;
		
		// Ctrl + C 방지 기능
    	sigemptyset(&new);
    	sigaddset(&new, SIGINT); // 시그널 집합에 SIGINt 시그널 설정
    	// 블록된 시그널을 해제하면 기다리고 있던 SIGQUIT 시그널을 받은 프로세스 
    	// 코어 덤프 생성 후 종료
    	sigprocmask(SIG_BLOCK, &new, (sigset_t *)NULL); 

		sleep(600); // 로그인 실패. 이 후 600초 동안 로그인 시도할 수 없음.
		//clear();
	}
	refresh();
	getch();
	endwin();
}

int main(int argc, char *argv[]){
	int nTry;
	char buf_login[BUFLEN_MAX],buf_password[BUFLEN_MAX];

	//아이디 비밀번호 
	const char *user_login = "pyd6118";
	const char *user_password = "asd123";

	struct sigaction sa;

	pid_t pid;	
	
	if((pid=fork())<0) { //fork error
		printf("Fork error\n");
		exit(1);
	}

	else if(pid==0) { //child process
		
		initscr(); // ncurses 자료구조들을 초기화, 적당한 terminfo 파일 읽는데 사용됨.

		memset(&sa,0,sizeof(struct sigaction));

		sa.sa_handler=sig_handler;
		sigfillset(&sa.sa_mask);
		sa.sa_flags=0;
		sigaction(SIGALRM,&sa, NULL);
	
		g_remain=EXPIRE_TIME;
		g_timer_param=EXPIRE_TIME;

		alarm(0);
        alarm(EXPIRE_TIME);

		nTry=0;
		while(nTry <TRY_MAX){
			
			draw_interface(nTry);
			
			if(get_string(MODE_ID, buf_login) <= 0)
				nTry++;

			else{
				if(get_string(MODE_PASSWORD, buf_password) <= 0)
					nTry++;

				else{//id, password 비교
					if(!strcmp(buf_login, user_login)&& !strcmp(buf_password, user_password)) break;
					else nTry++;
		
				}
			}
		}
		endwin();
        move(INIT_Y, INIT_X);

		if(nTry < TRY_MAX) 
            kill(pid, SIGUSR1);
        else 
            kill(pid, SIGUSR2);
        
	}

	//parent process
	else{

		wait(NULL);

        if(signal(SIGUSR1, p_handler) == SIG_ERR)
            perror("SIGUSR1 ERROR\n");
        if(signal(SIGUSR2, p_handler) == SIG_ERR)
            perror("SIGUSR2 ERROR\n");

		
	}
	return 0;
}