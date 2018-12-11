/*
 * Написать программу, в которой предок и потомок обмениваются сообщением через программный канал.
 */
#include <stdio.h> //printf
#include <stdlib.h> //exit
#include <unistd.h> //pipe
#include <string.h> //strlen
#include <signal.h>
#include <time.h>

int parent_flag = 0;

void sigint_catcher(int signum)
{
    parent_flag = 1;
}

int main(int argc, char *argv[])
{
	unsigned size = argc < 2 ? 2 : strtol(argv[1], NULL, 10), pos = 0;
	pid_t pids[size];

	int descr[2]; 

	signal(SIGINT, sigint_catcher);

	if (pipe(descr) == -1)
	{
        	perror( "Couldn't pipe." );
		exit(1);
	}

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
			printf("child: waiting for CTRL+C signal for 5 seconds...\n");
			char msg[64];
			
			close(descr[0]);
			sleep(5);
			if (parent_flag)
			{
				snprintf(msg, 64, "Child %d", pos);
				write(descr[1], msg, strlen(msg));
			}
			exit(0);
		}
	} while(pids[pos] && ++pos < size);
	
	if (pids[0] && pids[1])
	{
		char msg1[64];
		close(descr[1]); 
		read( descr[0], msg1, 64);
		sleep(2);
		char msg2[64];
		read( descr[0], msg2, 64);

		printf("%s %s\n", msg1, msg2);

	}

	return 0;
}
