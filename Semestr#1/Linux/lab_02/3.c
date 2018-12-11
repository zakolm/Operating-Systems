/*
 * Написать программу, в которой процесс-потомок вызывает системный вызов exec, а процесс-предок ждет завершения процесса-потомка.
 */
#include <stdio.h> //printf
#include <stdlib.h> //strtol
#include <unistd.h> //fork, getpid, getppid, execlp
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
			if (!(pos % 2))
			{
				if (execlp("ps", "ps", "al", NULL) == -1)
				{
					perror("couldn't exec.");
					exit(1);
				}
			}
			else
			{
				if (execlp("echo", "echo", "\"HELLO\"", NULL) == -1)
				{
					perror("couldn't exec.");
					exit(1);
				}
			}
		}
		else
		{
			printf("\x1b[0m* parent: pid: %d, child: %d\n", getpid(), pids[pos]);
			printf("\x1b[0m* parent: waiting for child to finish...\n");

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
		}
	} while(pids[pos] && ++pos < size); // parent proc
	
	printf("\x1b[0m\n");

	return 0;
}
