#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<pthread.h>
#include<time.h>
 
#include <pwd.h>
#include <sys/types.h>
#include <time.h>

#include <signal.h>
#include <sys/wait.h>

#include <openssl/aes.h>
#include <openssl/evp.h>
#include <openssl/err.h>
//#include <proc/readproc.h>

#define BUF_SIZE 128
#define NORMAL_SIZE 20
 
void* send_msg(void* arg);
void* recv_msg(void* arg);
void error_handling(char* msg);
 
void menu();
void changeName();
void menuOptions(); 
void userInfo(); 
void encryptMode();

int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *ckey,
            unsigned char *ivec, unsigned char *ciphertext);
int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
            unsigned char *iv, unsigned char *plaintext);
void handleErrors(void);


char name[NORMAL_SIZE]="[DEFALT]";     // name
char msg_form[NORMAL_SIZE];            // msg form
char serv_time[NORMAL_SIZE];        // server time
char msg[BUF_SIZE];                    // msg
char serv_port[NORMAL_SIZE];        // server port number
char clnt_ip[NORMAL_SIZE];            // client ip address
int tmp = 0;
char tmpText[256];

//unsigned char ckey[] = "thiskeyisverybaddontusethisinput"; // key 값
//unsigned char ivec[] = "dontusethisinputaatwosdfjjienone"; // IV 값
unsigned char ckey[] = "01234567890123456789012345678901"; // key 값
unsigned char ivec[] = "0123456789012345"; // IV 값

 typedef struct {
    unsigned long size,resident,share,text,lib,data,dt;
} statm_t;
 


int main(int argc, char *argv[])
{
     int sock;
    pthread_t snd_thread, rcv_thread;
    struct sockaddr_in serv_addr;
    void* thread_return;

    if (argc!=4)
    {
        printf(" Usage : %s <ip> <port> <name>\n", argv[0]);
        exit(1);
    }
 
    /** local time **/
    struct tm *t;
    time_t timer = time(NULL);
    t=localtime(&timer);
    sprintf(serv_time, "%d-%d-%d %d:%d", t->tm_year+1900, t->tm_mon+1, t->tm_mday, 
    t->tm_hour, t->tm_min);
 

    sprintf(name, "[%s]", argv[3]);
    sprintf(clnt_ip, "%s", argv[1]);
    sprintf(serv_port, "%s", argv[2]);
    sock=socket(PF_INET, SOCK_STREAM, 0);
 
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
    serv_addr.sin_port=htons(atoi(argv[2]));
 
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
        error_handling(" conncet() error");
 
    /** call menu **/
    menu();
 
    pthread_create(&snd_thread, NULL, send_msg, (void*)&sock);
    pthread_create(&rcv_thread, NULL, recv_msg, (void*)&sock);
    pthread_join(snd_thread, &thread_return);
    pthread_join(rcv_thread, &thread_return);
    close(sock);
    return 0;
}
 
void* send_msg(void* arg)
{
    int sock=*((int*)arg);
    char name_msg[NORMAL_SIZE+BUF_SIZE];
    char myInfo[BUF_SIZE];
    char* who = NULL;
    char temp[BUF_SIZE];
 
    /** send join messge **/
    sprintf(myInfo, "%s님이 입장했습니다. \n",name );
    //printf(" >> 1 \n");
    write(sock, myInfo, strlen(myInfo));
    //printf(" >> 2 \n");

    while(1)
    {
        //printf(" >> 3 \n");
        fgets(msg, BUF_SIZE, stdin); //
        //printf(" >> 4 \n");
        // 메뉴 선택
        if (!strcmp(msg, "go\n"))
        {
            menuOptions();
            continue;
        }
 
        else if (!strcmp(msg, "q\n") || !strcmp(msg, "Q\n"))
        {
            system("clear");
            close(sock);
            exit(0);
        }

        // send message
        //aes_256_cbc(msg, ckey, ivec);
        sprintf(name_msg, "%s %s", name,msg);
        write(sock, name_msg, strlen(name_msg));
    }
    return NULL;
}
 
void* recv_msg(void* arg)
{
    int sock=*((int*)arg);
    char name_msg[NORMAL_SIZE+BUF_SIZE];
    int str_len;

    char cipherText[NORMAL_SIZE+BUF_SIZE];
    int plaintext_len;
    int ciphertext_len;

    while(1)
    {
        str_len=read(sock, name_msg, NORMAL_SIZE+BUF_SIZE-1);
        if (str_len==-1)
            return (void*)-1;
        name_msg[str_len]=0;

        
        fputs(name_msg, stdout);
        
    }
    return NULL;
}
 
 
void menuOptions() 
{
    int select;
    system("clear");
    printf("\n\t**** 메뉴창 ****\n");
    printf("\t1. 사용자 이름 변환\n");
    printf("\t2. 재접속\n");
    printf("\t3. 방 주인 정보 확인\n");
    printf("\t4. 나에게 보내기\n");
    printf("\t위에 있는 키들만 입력할 수 있습니다.");
    printf("\n\t*******************");
    printf("\n\t>> ");
    scanf("%d", &select);
    getchar();
    switch(select)
    {
        // change user name
        case 1:
        changeName();
        break;
 
        // console update(time, clear chatting log)
        case 2:
        menu();
        break;
    
        case 3:
        userInfo();
        break;

        case 4:
        encryptMode();
        break;

        // menu error
        default:
        printf("\tcancel.");
        system("clear");
        menuOptions();
        
        
        break;
    }
}
 
 
/** change user name **/
void changeName()
{
    char nameTemp[100];
    printf("\t이름 변경 : ");
    scanf("%s", nameTemp);
    sprintf(name, "[%s]", nameTemp);
    printf("\t완료\n\n");
}
 
void menu()
{
    system("clear");
    printf(" **** 채팅이 접속되었습니다. ****\n");
    printf(" 서버 포트 번호 : %s \n", serv_port);
    printf(" 접속자 IP   : %s \n", clnt_ip);
    printf(" 서버 시간 : %s \n", serv_time);
    printf(" 메뉴창으로 가기 : go\n");
    printf(" 종료 : q / Q\n\n");
    
}    
 
void error_handling(char* msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}

void userInfo() 
{ 
    //사용자 uid를 통해서 pw 구조체를 불러오고, 해당 정보를 출력
	struct passwd *pw;
	uid_t uid;
	uid=getuid();
	pw=getpwuid(uid);
	printf("My Information Name:[%s], Uid:[%d], Gid[%d], Home[%s]\n",
		pw->pw_name, pw->pw_uid, pw->pw_gid, pw->pw_dir);
   
	return;
}

void encryptMode()
{
    char buf[256];
    char *arg[7];
    char *s;
    char *save;
    int argv;
    int flag = 0;

    /* 공백, 탭, 개행으로 이루어진 구분자 선언 */
    static const char delim[] = " \t\n";
    int pid, status;

    char cipherText[128];
    int plaintext_len;
    int ciphertext_len;

    unsigned char decryptedText[256];
    int decryptedtext_len;
    
    int signum;
    siginfo_t info;
	sigset_t set; // 시그널

    /* 무한 반복 */
    while(1) {
       /* 프롬프트 출력 */

       sigemptyset(&set);
	   sigaddset(&set, SIGINT); // SIGINT 시그널 번호를 set에 추가
	   // 기존에 블록된 시그널이 있다면 set signal을 추가 / 초기화라고 생각하기
	   sigprocmask(SIG_BLOCK, &set, NULL); 

       printf("나에게 보내기: ");
       fgets(buf, 256, stdin);
       argv = 0;
       

       plaintext_len = strlen(buf);
       ciphertext_len = encrypt(buf, plaintext_len, ckey, ivec, cipherText);
       printf("메시지 암호화 \n");
       BIO_dump_fp (stdout, (const char *)cipherText, ciphertext_len);
       
       decryptedtext_len = decrypt(cipherText, ciphertext_len, ckey, ivec, decryptedText);
       decryptedText[decryptedtext_len] = '\0';
       
       printf("암호문 복호화 : %s", decryptedText);
        

       /* 문자열에서 delim을 기준으로 단어를 잘라냄 */
       s = strtok_r(buf, delim, &save);

       while(s) {
          arg[argv++] = s;
          s = strtok_r(NULL, delim, &save);
       }

       /* 인수가 더 없음을 의미하는 문자 추가 */
       arg[argv] = (char *)0;

       /* 프롬프트로 입력 받은 첫 번째 단어가 "exit" while 문 벗어남 */
       if (!strcmp(arg[0], "exit")) {
           system("clear");
           signum = sigwaitinfo(&set, &info);
           while(signum == SIGINT); // signum과 SIGINT와 다를 시 프로그램 종료.
           return;
       }
       if ((pid=fork()) == -1)   /* fork 호출에 실패하면 */
          perror("fork failed");

       /* 부모 프로세스는 자식 프로세스가 종료되기를 기다림 */
       else if (pid != 0) {
          pid = wait(&status);

       /* 자식 프로세스는 execvp를 이용하여 arg[0] 실행 */
       } else {
          execvp(arg[0], arg);
       }
    }
    exit(0);
}

int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key,
            unsigned char *iv, unsigned char *ciphertext)
{
    EVP_CIPHER_CTX *ctx;

    int len;

    int ciphertext_len;

    
    /* Create and initialize the context */
    if(!(ctx = EVP_CIPHER_CTX_new()))
        handleErrors();
    /*
     * Initialise the encryption operation. IMPORTANT - ensure you use a key
     * and IV size appropriate for your cipher
     * In this example we are using 256 bit AES (i.e. a 256 bit key). The
     * IV size for *most* modes is the same as the block size. For AES this
     * is 128 bits
     */
    if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
        handleErrors();

    
    /*
     * Provide the message to be encrypted, and obtain the encrypted output.
     * EVP_EncryptUpdate can be called multiple times if necessary
     */
    if(1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
        handleErrors();
    ciphertext_len = len;

    /*
     * Finalise the encryption. Further ciphertext bytes may be written at
     * this stage.
     */
    if(1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len))
        handleErrors();
    ciphertext_len += len;

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    return ciphertext_len;
}

int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
            unsigned char *iv, unsigned char *plaintext)
{
    EVP_CIPHER_CTX *ctx;

    int len;

    int plaintext_len;
    //printf("1\n");
    /* Create and initialise the context */
    if(!(ctx = EVP_CIPHER_CTX_new()))
        handleErrors();
    //printf("2\n");
    /*
     * Initialise the decryption operation. IMPORTANT - ensure you use a key
     * and IV size appropriate for your cipher
     * In this example we are using 256 bit AES (i.e. a 256 bit key). The
     * IV size for *most* modes is the same as the block size. For AES this
     * is 128 bits
     */
    if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
        handleErrors();
    //printf("3\n");
    /*
     * Provide the message to be decrypted, and obtain the plaintext output.
     * EVP_DecryptUpdate can be called multiple times if necessary.
     */
    if(1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
        handleErrors();
    plaintext_len = len;
    //printf("4\n");
    /*
     * Finalise the decryption. Further plaintext bytes may be written at
     * this stage.
     */
    if(1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len))
        handleErrors();
    plaintext_len += len;
    //printf("5\n");
    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    return plaintext_len;
}

void handleErrors(void)
{
    ERR_print_errors_fp(stderr);
    abort();
}