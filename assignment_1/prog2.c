#include <stdlib.h>
#include <stdio.h>

int main(void) {
         FILE *rfp, *wfp;
         int id, s1, s2, s3, n;

         if ((rfp = fopen("data", "r")) == NULL) {
                 perror("fopen: data");
                 exit(1);
         }
        
         if((wfp = fopen("data.scr", "w")) == NULL) {
                 perror("fopen: data.scr");
                 exit(1);
         }

         while ((n=fscanf(rfp, "%d %d %d %d", &id, &s1, &s2, &s3)) != EOF) {
                 fprintf(wfp, "%d %d\n", id, (s1+s2+s3)/3);
         }


         fclose(rfp);
         fclose(wfp);
         return 0;
}