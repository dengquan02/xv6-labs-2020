#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"

#define MAX_LINE 512

int
main(int argc, char *argv[])
{
  char buf[MAX_LINE], c;
  int nread, total_read = 0;

	// if(argc < 2){
	// 	fprintf(2, "Usage: xargs ...\n");
	// 	exit(1);
	// }

  /*从标准输入中按行读取*/
  while ((nread = read(0, &c, 1)) > 0) {
    if (c == '\n') {
      buf[total_read] = 0;
      if (fork() == 0) {
        int new_argc = argc;
        char **new_argv = (char **)malloc(sizeof(char*) * (new_argc + 1));
        if (!new_argv) {
          fprintf(2, "malloc error\n");
          exit(1);
        }

        for (int i = 1; i < argc; ++i) {
          new_argv[i - 1] = argv[i];
        }
        new_argv[argc - 1] = buf;
        exec(new_argv[0], new_argv);
      }
      else {
        wait(0);
      }
      total_read = 0;
    }
    else {
      buf[total_read ++] = c;
      if (total_read > MAX_LINE - 1) {
        buf[MAX_LINE - 1] = 0;
        fprintf(2, "Warning: Line too long, truncating.\n");
        total_read = 0;
      }
    }
  }

  if (nread < 0) {
    fprintf(2, "Error: read wrong.\n");
    exit(1);
  }

  exit(0);
}