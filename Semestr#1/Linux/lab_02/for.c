/*
 * Написать программу, запускающую новый процесс системным вызовом fork().
 * В предке вывести собственный идентификатор(getpid), идентификатор потомка(элемент массива pids) и идентификатор группы(getpgrp).
 * В процессе-потомке вывести собственный идентификатор(getpid), идентификатор предка(getppid) и идентификатор группы(getpgrp).
 * Убедиться, что при завершении процесса-предка(sleep) потомок получает идентификатор предка (PPID), равный 1.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
        unsigned size = argc < 2 ? 2 : strtol(argv[1], NULL, 10);
        pid_t pids[size];

        // для первичного входа в цикл используем условие !i(при объявлении он равен 0)  т.к. не было первого fork
        for (unsigned i = 0; (i < size && pids[i]) || !i; ++i)
        {
                pids[i] = fork();
                if (pids[i] == -1)
                {
                        perror("Can't fork!\n");
                        exit(1);
                }
                else if (pids[i] == 0)
                {
                        printf("\x1b[93m- child: pid: %d, ppid: %d\n", getpid(), getppid());
                        sleep(2);
                        printf("\n\x1b[93m- child: pid: %d, ppid: %d\n", getpid(), getppid());
                        return 0;
                }
                else
                {
                        printf("\x1b[0m* parent: pid: %d, child: %d\n", getpid(), pids[i]);
                }
        }

        printf("\x1b[0m\n");

        return 0;
}

