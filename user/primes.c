#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
	int prime, num, send, p1[2], p2[2];

	pipe(p1);
	for (int i = 2; i <= 35; ++i)
		write(p1[1], &i, 4);
	close(p1[1]);

start:
	prime = 0;
	if (read(p1[0], &prime, 4) <= 0) {
		fprintf(2, "error!\n");
		exit(1);
	}
	printf("prime %d\n", prime);
	send = 0;

	while (read(p1[0], &num, 4) > 0) {
		if (num % prime) {
			if (!send) {
				send = 1;
				pipe(p2);
				/*pipe后可以确定的一点是：
				  p1[0] p2[0] p2[1] 分别是{3,4,5}中的一个各不相同的数 */
				if (fork() == 0) {
					/*close的两个文件描述符，将分配给下次fork*/
					close(p1[0]);
					close(p2[1]);
					p1[0] = p2[0];
					goto start;
				}
				else {
					close(p2[0]);
					p1[1] = p2[1];
				}
			}
			write(p1[1], &num, 4);
		}
	}
	/*close要在wait之前，否则子进程将因为管道的写入文件描述符未关闭而阻塞在read*/
	close(p1[0]);
	close(p1[1]);
	wait(0);
	exit(0);
}