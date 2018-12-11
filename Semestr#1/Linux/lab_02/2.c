/*
 * Написать программу по схеме первого задания, но в процессе-предке выполнить системный вызов wait().
 * Убедиться, что в этом случае идентификатор процесса потомка на 1 больше идентификатора процесса-предка.
*/
#include <stdio.h> //printf
#include <stdlib.h> //strtol
#include <unistd.h> //fork, getpid, getppid
#include <sys/wait.h> //wait

int main(int argc, char *argv[])
{
	unsigned size = argc < 2 ? 2 : strtol(argv[1], NULL, 10), pos = 0;
	pid_t pids[size];

	do
	{
		// point fork
		pids[pos] = fork();

		if (pids[pos] == -1)
		{
			perror("Can't fork!\n");
			exit(1);
		}
		else if (pids[pos] == 0)
		{
			printf("\x1b[93m- child: pid: %d, ppid: %d\n", getpid(), getppid());
			int res = getpid() % 2;
			printf( "\x1b[93m- child: returning %d.", res );
			printf( "\x1b[0m\n" );
			return res;
		}
		else
		{
			printf("\x1b[0m* parent: pid: %d, child: %d\n", getpid(), pids[pos]);
			int stat = 0;
			pid_t res = wait(&stat);
			if (WIFEXITED(stat))
			{
				printf("\x1b[0m* parent: child %d finished with %d code.\n", res, WEXITSTATUS(stat) );
			}
			else
			{
				printf("\x1b[0m* parent: child finished abnormally.\n" );
			}
			return 0;
		}
	} while(pids[pos] && ++pos < size); // parent proc
	
	printf("\x1b[0m\n");

	return 0;
}
