#include <stdio.h>
#include <string.h>
#include <grp.h>

int main(int argc, char *argv[])
{
   struct group *grp;
   int n;
   char userName[256];

   printf("Input user name ");
   scanf("%s", userName);

   grp = getgrnam(userName); // userName에 대한 그룹 정보 검색
   printf("User : %s\n", userName);
   printf("Primary Group : %s\n", grp->gr_name);

   printf("Secondary Group : ");

   while((grp = getgrent()) != NULL) // secondary group에대한 정보 읽어오기
   {
      n = 0; // 초기화
      while(grp->gr_mem[n] != NULL)
      {
         if(!strcmp(userName, grp->gr_mem[n])) // primary group 정보는 X
            printf("%s ", grp->gr_name);
         
         n += 1;
      }
   }

   printf("\n");

   return 0;

}