#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include <sys/shm.h>

// Операции на семафорах
#define SEM_OP_INC 1
#define SEM_OP_DEC -1
#define SEM_OP_WAIT 0

// Права доступа
#define PERM_ALL (S_IRWXU | S_IRWXG | S_IRWXO)

// Ключи семафор
#define RR	3	//ждущие читатели     WW
#define SB	2	//бинарный            AW
//#define WR	1	//ждущие писатели     WR
#define RI	0	//активные писатели   AR

// Максимальное время задержки для читателей и писателей
#define MAX_WRITER_SLEEP 5
#define MAX_READER_SLEEP 10

// Количество процессов для читателей и писателей
#define WRITER_CNT 3
#define READER_CNT 5
#define ITER 20

struct sembuf read_start[]  =  {{SB, SEM_OP_WAIT, 0}, 
                                {RI, SEM_OP_INC, 0}};
                               
struct sembuf read_stop[]   =  {{RI, SEM_OP_DEC, 0}};
                               
struct sembuf write_start[] =  {{RR, SEM_OP_INC,0},
                                {RI, SEM_OP_WAIT, 0}, 
                                {SB, SEM_OP_WAIT, 0},
								{SB, SEM_OP_INC, 0}, 
                                {RR, SEM_OP_DEC, 0}};
                               
struct sembuf write_stop[]  =  {{SB, SEM_OP_DEC, 0}};
                                

int* pdata = NULL;
int sem_fd = -1;
 
void startRead()
{ 
	semop(sem_fd, read_start, 2); 
}

void stopRead()
{ 
	semop(sem_fd, read_stop, 1);
}

void startWrite()
{
	semop(sem_fd, write_start, 5);
}

void stopWrite()
{ 
	semop(sem_fd, write_stop, 1); 
}

void reader(int reader_id)
{
    int i;
    for (i = 0; i < ITER; ++i)
    {
        sleep(3);
		startRead();
		printf ("Reader%d read %d\n", reader_id, *pdata);
		stopRead();
	}  
}

void writer(int writer_id)
{   
    int i;
    for (i = 0; i < ITER; ++i)
    {
        sleep(2);
		startWrite();
		printf("Writer%d write %d\n", writer_id, ++(*pdata));
		stopWrite();
	}  
}


int main()
{
	int shm_fd = -1;
    int childs[READER_CNT + WRITER_CNT];
    int i;
    int status;
	
	/* Создать разделяемый сегмент */
	shm_fd = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | PERM_ALL);
	if (shm_fd == -1)
	{
		perror("Error: shmget!\n");
		exit(1);
	}
	
	/* Получить разделяемый сегмент */
	pdata = (int*)shmat(shm_fd, 0, 0);
    (*pdata) = 0;
    
	if (pdata == (int*)-1)
	{
		perror("Error: shmat!\n");
		exit(1);
	}
	
	/* Создать набор семафор */
	sem_fd = semget(IPC_PRIVATE, 4, IPC_CREAT | PERM_ALL);
	if (sem_fd == -1)
	{
		perror("Error: semget!\n");
		exit(1);
	}
	/* Установить управляющие параметры - начальные значения */
    semctl(sem_fd, RR, SETVAL, 0);
    semctl(sem_fd, SB, SETVAL, 0);
    semctl(sem_fd, RI, SETVAL, 0);
    //semctl(sem_fd, WR, SETVAL, 0);
    

    /* Создать дочерние процессы */
	for (i = 0; i < READER_CNT + WRITER_CNT; ++i)
	{
		childs[i] = fork();
        
		if (childs[i] == -1)
		{
			perror("Can't fork\n"); 
			exit(1);
		}
		else if (childs[i] == 0)
		{
            if (i < WRITER_CNT)
            {
                writer(i);
            }
            else if (i >= WRITER_CNT && i < READER_CNT + WRITER_CNT)
            {
                reader(i - WRITER_CNT);              
            }
			exit(0);
		}
	}

    /* Ожидать окончание всех дочерних процессов */
	for (i = 0; i < READER_CNT + WRITER_CNT; ++i)
	{
		wait(&status);
	}
    
	/* Отсоединить разделяемый сегмент */
	if (shmdt(pdata) == -1)
	{
		perror("Error: shmdt!\n");
	}
	
	return EXIT_SUCCESS;
}
