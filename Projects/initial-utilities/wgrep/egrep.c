#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int main(int argc, char *argv[]) {
    if (!argv[1]) {
        printf( "wgrep: searchterm [file ...]\n");
        exit(1);
    }

    if (!argv[2]) {
        char *buffer = NULL;
        size_t buffer_size = 0;
        ssize_t line_length;

        char *ret;

        line_length = getline(&buffer, &buffer_size, stdin);
        while (line_length >= 0) { 
            ret = strstr(buffer, argv[1]);
            if (ret)
                printf("%s", buffer);

            line_length = getline(&buffer, &buffer_size, stdin);
        }
    }

    for(int a = 2; a < argc; a++ ){
      FILE *fp = fopen(argv[a], "r");
      if (fp == NULL) {
          printf("wgrep: cannot open file\n");
          exit(1);
      }
      else{
          char *buffer = NULL;
          size_t buffer_size = 0;
          ssize_t line_length;

          char *ret;

          line_length = getline(&buffer, &buffer_size, fp);
          while (line_length >= 0) {         

            ret = strstr(buffer, argv[1]);
            if (ret)
                printf("%s", buffer);

            line_length = getline(&buffer, &buffer_size, fp);
          }

          fclose(fp);
      }
   }
   exit(0);
   return 0;
}

