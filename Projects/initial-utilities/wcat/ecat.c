#include <stdio.h>
#include <stdlib.h>
#define MAX_SIZE 1000

int main(int argc, char *argv[]) {
   for(int a = 1; a < argc; a++ ){
      FILE *fp = fopen(argv[a], "r");
      if (fp == NULL) {
          printf("ecat: cannot open file\n");
          exit(1);
      }
      else{
          char buffer[MAX_SIZE];
          fgets(buffer, MAX_SIZE, fp);
          while(!feof(fp)) {
              printf("%s", buffer);
              fgets(buffer, MAX_SIZE, fp);
          }
          fclose(fp);
      }
   }
   exit(0);
   return 0;
}

