#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    struct stat buf;
    int pmode;
    
    if(argc != 3) { // 명령어 단어가 3개가 아닌 경우
        printf("execute");
        exit(1);
    }

    stat(argv[2], &buf);

    switch(argv[1][0]) {
        case 'u': // user
            switch(argv[1][2]) {
                case 'r': 
                    pmode = S_IRUSR; 
                    break;
                case 'w': 
                    pmode = S_IWUSR; 
                    break;
                case 'x': 
                    pmode = S_IXUSR; 
                    break;
                default :
                    printf("Permission error\n");
                    exit(1);
            }
            break;
        case 'g': // group
            switch(argv[1][2]) {
                case 'r': 
                    pmode = S_IRGRP; 
                    break;
                case 'w': 
                    pmode = S_IWGRP; 
                    break;
                case 'x': 
                    pmode = S_IXGRP; 
                    break;
                default :
                    printf("Permission error\n");
                    exit(1);
            }
            break;
        case 'o':
            switch(argv[1][2]) {
                case 'r': 
                    pmode = S_IROTH; 
                    break;
                case 'w': 
                    pmode = S_IWOTH; 
                    break;
                case 'x': 
                    pmode = S_IXOTH; 
                    break;
                default :
                    printf("Permission mode error\n");
                    exit(1);
            }
            break;
        default :
            printf("Permission error\n");
            exit(1);
    }
    if(argv[1][1] == '+') 
        buf.st_mode |= pmode; 
    else if(argv[1][1] == '-') 
        buf.st_mode &= ~(pmode); 

    chmod(argv[2], buf.st_mode);

    return 0;
}
