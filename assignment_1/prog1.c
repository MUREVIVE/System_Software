#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main(void)
{
    int i=0, j=0, score, fd;
    int g[2];
    char buf[10];

    fd=open("data", O_RDONLY);
    if(fd == -1) 
    {
        perror("Open data");
        exit(1);
    }

    while(read(fd, buf, 7) > 0)
    {
        buf[7]='\0';
        g[i]=atoi(buf);
            
        score = 0;
        for(int t=0; t<3; t++)
        {
            read(fd, buf, 1);
            read(fd, buf, 2);
            buf[2] = '\0';
            score += atoi(buf);
        }

        printf("%d %d\n", g[i], score / 3);
        read(fd, buf, 1);
        i++;
    }

    close(fd);
    return 0;
}
