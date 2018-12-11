/*
 * Написать программу, в которой предок и потомок обмениваются сообщением через программный канал.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[])
{
	unsigned size = argc < 2 ? 2 : strtol(argv[1], NULL, 10), pos = 0;
	int descr[2];
	pid_t pids[size];

	if ( pipe(descr) == -1)
	{
		perror( "couldn't pipe." );
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

			char msg[64];// = "hello";
			if (!(pos % 2))
			{
				snprintf(msg, 64, "hello");
			}
			else
			{
				snprintf(msg, 64, "world");
			}
			
			close(descr[0]);
			write(descr[1], msg, sizeof(msg));
			
			printf("- child: write <%s>\n", msg);
			return 0;
		}
	} while(pids[pos] && ++pos < size); // parent proc
	
		if (pids[0] && pids[1])
		{
			char msg1[64];
			close( descr[1] );
			read( descr[0], msg1, 64);
			sleep(2);
			char msg2[64];
			read( descr[0], msg2, 64);

			printf("Parent: reads %s %s", msg1, msg2);

		}

	return 0;
}
