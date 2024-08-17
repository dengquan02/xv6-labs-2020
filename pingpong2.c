/* 1.6 练习
 * 编写一个使用UNIX系统调用的程序，
 * 通过一对管道在两个进程之间“ping-pong”一个字节（也就是像打乒乓球一样来回传递），
 * 每个方向一个管道。
 * 以每秒的交换次数为单位，测量程序的性能。
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

int
main(int argc, char *argv[])
{
  int buf[1], p1[2], p2[2], cnt = 0, start = clock();

  pipe(p1);
  pipe(p2);
  
  if (fork() == 0) {
    close(p1[1]);
    close(p2[0]);
    while ((clock() - start) < CLOCKS_PER_SEC) {
      read(p1[0], buf, sizeof(buf));
      // printf("%d <%d>: received ping\n", ++cnt, getpid());
      write(p2[1], " ", 1);
    }
    close(p1[0]);
    close(p2[1]);
  }
  else {
    close(p1[0]);
    close(p2[1]);
    while ((clock() - start) < CLOCKS_PER_SEC) {
      write(p1[1], " ", 1);
      read(p2[0], buf, sizeof(buf));
      // printf("%d <%d>: received pong\n", ++cnt, getpid());
      ++cnt;
    }
    printf("%d\n", cnt);
    close(p1[1]);
    close(p2[0]);
  }
  exit(0);
}
