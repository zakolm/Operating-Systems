/*
 * Написать программу, запускающую новый процесс системным вызовом fork().
 * В предке вывести собственный идентификатор(getpid), идентификатор потомка(элемент массива pids) и идентификатор группы(getpgrp).
 * В процессе-потомке вывести собственный идентификатор(getpid), идентификатор предка(getppid) и идентификатор группы(getpgrp).
 * Убедиться, что при завершении процесса-предка(sleep) потомок получает идентификатор предка (PPID), равный 1.
*/
#include <stdio.h> //printf
#include <stdlib.h> //strtol
#include <unistd.h> //fork, getpid, getppid

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
			perror("Can't fork!");
			exit(1);
		}
		else if (pids[pos] == 0)
		{
			printf("- child: pid: %d, ppid: %d\n", getpid(), getppid());
			printf("- child waiting");
			sleep(5);
			printf("- child: pid: %d, ppid: %d\n", getpid(), getppid());
		}
		else
		{
			printf("* parent: pid: %d, child: %d\n", getpid(), pids[pos]);
		}
	} while(pids[pos] && ++pos < size); // parent proc
	
	printf("\x1b[0m\n");

	return 0;
}
