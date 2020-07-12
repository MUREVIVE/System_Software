#include <sys/types.h> 
#include <sys/socket.h> 
#include <unistd.h> 
#include <arpa/inet.h> 
#include <stdlib.h> 
#include <stdio.h> 
#include <string.h> 

#include <netdb.h>

#define PORTNUM 9000 

int main(void) { 

    int sd; 
    char buf[256]; 
    struct sockaddr_in sin; 
    struct hostent *hp;
    // 서버와 마찬가지로 소켓을 생성한다. 
    
    if((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) { 
        perror("socket"); 
        exit(1); 
    } 
    
    // 소켓 주소 구조체를 설정한다. 
    // 이때 IP 주소는 서버의 주소로 설정해야 한다. 
    memset((char *)&sin, '\0', sizeof(sin)); 
    sin.sin_family = AF_INET; 
    sin.sin_port = htons(PORTNUM);
    sin.sin_addr.s_addr = inet_addr("10.0.2.15"); 

    hp = gethostbyaddr((char*)&sin.sin_addr.s_addr, 4, AF_INET);
     // connect 함수로 서버에 연결을 요청한다. 
     if(connect(sd, (struct sockaddr *)&sin, sizeof(sin))) { 
         perror("bind: "); 
         exit(1); 
     } 
     
     // 연결되면 서버에서 오는 메시지를 받는다. 
     if(recv(sd, buf, sizeof(buf), 0) == -1) { 
         perror("recv"); 
         exit(1); 
     } 
     
    close(sd); 
    
    printf("From Server - Host Name : %s\n", hp->h_name);
    printf("From Server - IP Address : %s\n", inet_ntoa(sin.sin_addr));
    printf("From Server - Port Number : %d\n", ntohs(sin.sin_port));
     return 0; 

}

