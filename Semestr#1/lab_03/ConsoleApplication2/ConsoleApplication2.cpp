#include "pch.h"

#include <stdio.h>
#include <time.h>
#include <windows.h>

#define READERS 5
#define WRITERS 3

#define TURNS 5

#define READSLP 1500
#define WRITESLP 2500

HANDLE mutex_buff;
HANDLE mutex_print;

unsigned short int buffer = 0;

volatile LONG wait_readers = 0;
volatile LONG wait_writers = 0;

volatile LONG reading = 0;
volatile LONG writing = 1;

HANDLE can_read, can_write;

void start_read()
{
	InterlockedIncrement(&wait_readers);
	if (writing != 0 || wait_writers != 0) //a_w or w_w
	{
		WaitForSingleObject(can_read, INFINITE);
	}

	InterlockedDecrement(&wait_readers);
	InterlockedIncrement(&reading);

	SetEvent(can_read);
}

void stop_read()
{
	InterlockedDecrement(&reading);

	if (reading == 0) // a_r
	{
		SetEvent(can_write);
	}
}

void start_write()
{
	WaitForSingleObject(mutex_buff, INFINITE);

	InterlockedIncrement(&wait_writers);
	if (reading != 0 || writing != 0) // a_r or a_w
	{
		WaitForSingleObject(can_write, INFINITE);
	}

	InterlockedDecrement(&wait_writers);
	writing = 1;

	ReleaseMutex(mutex_buff);
}

void stop_write()
{
	writing = 0;

	if (wait_readers != 0) // 
	{
		SetEvent(can_read);
	}
	else
	{
		SetEvent(can_write);
	}
}

DWORD reader()
{
	srand(time(NULL));

	while (buffer < TURNS * WRITERS)
	{
		start_read();

		WaitForSingleObject(mutex_print, INFINITE);
		printf("Reader %ld read '%ld'\n", GetCurrentThreadId(), buffer);
		ReleaseMutex(mutex_print);

		Sleep(rand() % READSLP);

		stop_read();
	}

	return 0;
}

DWORD writer()
{
	srand(time(NULL));

	for (int i = 0; i < TURNS; i++)
	{
		start_write();

		buffer++;

		WaitForSingleObject(mutex_print, INFINITE);
		printf("\nWriter %ld wrote '%ld'\n\n", GetCurrentThreadId(), buffer);
		ReleaseMutex(mutex_print);

		Sleep(rand() % WRITESLP);

		stop_write();
	}

	return 0;
}

int main()
{
	HANDLE wThread[WRITERS];
	HANDLE rThread[READERS];

	// Создание MUTEX
	mutex_buff = CreateMutex(NULL, FALSE, NULL);
	if (mutex_buff == NULL)
	{
		printf("Couldn't create MUTEX!\n");
		return 1;
	}

	// Создание сигналов
	can_read = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (can_read == NULL)
	{
		printf("Couldn't create signal!\n");
		return 2;
	}

	can_write = CreateEvent(NULL, TRUE, TRUE, NULL);
	if (can_write == NULL)
	{
		printf("Couldn't create signal!\n");
		return 2;
	}

	for (int i = 0; i < READERS; i++)
	{
		rThread[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&reader, NULL, 0, NULL);
		if (rThread[i] == NULL)
		{
			printf("Couldn't create thread!\n");
			return 3;
		}
		Sleep(rand() % READSLP);
	}

	for (int i = 0; i < WRITERS; i++)
	{
		wThread[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&writer, NULL, 0, NULL);
		if (wThread[i] == NULL)
		{
			printf("Couldn't create thread!\n");
			return 3;
		}
		Sleep(rand() % WRITESLP);
	}

	// Ожидание завершения потоков
	WaitForMultipleObjects(WRITERS, wThread, TRUE, INFINITE);
	WaitForMultipleObjects(READERS, rThread, TRUE, INFINITE);

	CloseHandle(mutex_buff);
	CloseHandle(can_read);
	CloseHandle(can_write);

	system("pause");
	return 0;
}
