#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <netdb.h>

#define PORTNUM 9000

int main(void) {
    char buf[256];
    struct sockaddr_in sin, cli;
    struct hostent *hp;
    int sd, ns, clientlen = sizeof(cli);

    // socket 함수의 인자로 AF_INET과 SOCK_STREAM을 지정해 소켓을 생성한다.
    if((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    // 서버의 IP 주소를 지정하고, 포트 번호는 9000으로 지정해 소켓 주소 구조체를 설정한다.
    memset((char *)&sin, '\0', sizeof(sin));
    sin.sin_family  = AF_INET;
    sin.sin_port    = htons(PORTNUM);
    sin.sin_addr.s_addr = inet_addr("10.0.2.15");

    hp = gethostbyaddr((char*)&sin.sin_addr.s_addr, 4, AF_INET);
    
    // bind 함수로 소켓의 이름을 정하고 접속 요청을 받을 준비를 마쳤음을 알린다.
    if(bind(sd, (struct sockaddr *)&sin, sizeof(sin))) {
        perror("bind");
        exit(1);
    }

    if(listen(sd, 5)) {
        perror("listen");
        exit(1);
    }

    // accept 함수로 클라이언트의 요청을 수락한다.
    if((ns = accept(sd, (struct sockaddr *)&cli, &clientlen)) == -1) {
        perror("accept");
        exit(1);
    }

    // send 함수로 메시지를 전송한다.
    if (send(ns, buf, strlen(buf) + 1, 0) == -1) {
        perror("send");
        exit(1);
    }

    // 사용을 마친 소켓을 모두 닫는다.
    close(ns);
    close(sd);
    printf("From Server - Host Name : %s\n", hp->h_name);
    printf("From Server - IP Address : %s\n", inet_ntoa(sin.sin_addr));
    printf("From Server - Port Number : %d\n", ntohs(sin.sin_port));
    return 0;
}

