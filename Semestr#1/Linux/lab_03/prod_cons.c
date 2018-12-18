#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int N = 10;
const int COUNT = 10;
const int PERM = IPC_CREAT | S_IRWXU | S_IRWXG | S_IRWXO;
const char* ERR_SHM = "Невозможно создать область";
const int CERR_SHM = -1;
const char* ERR_AT = "Невозможно подключиться к области";
const int CERR_AT = -2;
const char* ERR_SEM = "Невозможно создать семафор";
const int CERR_SEM = -3;
const char* ERR_FORK = "Невозможно создать процесс";
const int CERR_FORK = -4;
const char* ERR_DT = "Невозможно отключиться от области";
const int CERR_DT = -5;


int* sh_buf;
int* sh_pos;
// Семафоры:
// 0 - семафор потребителя, fullCount, семафор-счетчик заполненности буфера. [fullCount, ...]
// 1 - семафор производителя, emptyCount, семафор-счетчик свободности буфера. [..., emptyCount]
// 2 - бинарный семафор, useQueue. Показывает, находится ли очередь(массив) в обработке производителя или потребителя
// struct sembuf name { sem_id, sem_op, sem_flags}. применяет опрерацию sem_op к семафору sem_id
struct sembuf prod_start[2] = {{1, -1, 0}, {2, -1, 0}};
struct sembuf prod_stop[2] = {{0, 1, 0}, {2, 1, 0}};
struct sembuf cons_start[2] = {{0, -1, 0}, {2, -1, 0}};
struct sembuf cons_stop[2] = {{1, 1, 0}, {2, 1, 0}};

int main()
{
	int shmid, semid; // объявляем ID для разделяемого сегмента, массива семафоров
	if ((shmid = shmget(IPC_PRIVATE, (N + 1)*sizeof(int), PERM)) == -1) // создаем разделяемый сегмент в локальном пр-ве процеса(IPC_PRIVATE), длинной (N + 1)*sizeof(int) - длина буфера(массива) + 1, с правами доступа PERM(см. выше)
	{
		printf("%s\n", ERR_SHM);
		return CERR_SHM;
	}
	sh_pos = shmat(shmid, 0, 0); // присоединяем разделяемый сегмент памяти, получив его начальный адрес
	sh_buf = sh_pos + sizeof(int); // сдвигаем на 1, чтобы длина буфера составила N
	(*sh_pos) = 0;
	if (*sh_buf == -1)
	{
		printf("%s\n", ERR_AT);
		return CERR_AT;
	}
	if ((semid = semget(IPC_PRIVATE, 3, PERM)) == -1) // создаем массив из 3х семафоров в локальном пр-ве процесса, с правами доступа PERM
	{
		printf("%s\n", ERR_SEM);
		return CERR_SEM;
	}
	semctl(semid, 1, SETVAL, N); // sem[1] = N
	semctl(semid, 2, SETVAL, 1); // sem[2] = 1
	pid_t pid;
	if ((pid = fork()) == -1)
	{
		printf("%s\n", ERR_FORK);
		return CERR_FORK;
	}
	if (pid == 0)
	{
		int k = 0;
		while (k < COUNT)
		{
			semop(semid, prod_start, 2); // совершаем неделимое действие с массивом семафоров
			sh_buf[*sh_pos] = rand()%10; // записываем в разделяемый сегмент памяти(массив) букву
			printf("Producer%d %d\n", k, sh_buf[*sh_pos]);
			(*sh_pos)++;
			semop(semid, prod_stop, 2);
			sleep(rand()%5);
			k++;
		}
	}
	else
	{
		int k = 0;
		while (k < COUNT)
		{
			semop(semid, cons_start, 2);
			(*sh_pos)--;
			printf("Consumer%d %d\n", k, sh_buf[*sh_pos]);
			semop(semid, cons_stop, 2);
			sleep(rand()%5);
			k++;
		}
	}
	if (pid != 0) 
	{
		wait();
		if (shmdt(sh_pos) == -1) // отсоединяем разделяемый сегмент памяти
		{
			printf("%s\n", ERR_DT);
			return CERR_DT;
		}
	}
}
