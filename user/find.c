#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  buf[strlen(p)] = 0;
  // memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
  return buf;
}

void
findRev(char *path, char *filename)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path, 0)) < 0){
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  /* 要求后面不加/ */
  switch(st.type){
  case T_FILE:
    // printf("%s --- %s --- %s\n", path, fmtname(path), filename);
    if (strcmp(fmtname(path), filename) == 0)
      printf("%s\n", path);
    break;

  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf("find: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0 || strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      findRev(buf, filename);
    }
    break;
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
	if(argc < 3){
		fprintf(2, "Usage: find <path> <filename>\n");
		exit(1);
	}

  if(open(argv[1], 0) < 0){
    fprintf(2, "find: cannot open %s\n", argv[1]);
    exit(2);
  }

  findRev(argv[1], argv[2]);
  exit(0);
}