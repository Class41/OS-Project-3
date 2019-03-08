//Vasyl Onufriyev
//3-6-19
//OS Spring 2019
//Project 3: Palindromes
//Purpose: Checks for palindromes

#include <stdio.h>
#include <stdlib.h>
#include "shared.h"
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <string.h>
#include <sys/types.h>
#include <pthread.h>
#include <semaphore.h>

int ipcid;
Shared* data;
char* filen;

void SetExit()
{
	shmdt(data);
	exit(21);
}

void WritePalin(int pos)
{
	sem_wait(&(data->pal));
	FILE* o = fopen("palin.out", "a");
	fprintf(o, "%i\t%i\t%s", getpid(), pos, data->rows[pos]);
	fclose(o);
	sem_post(&(data->pal));
}

void WriteNonPalin(int pos)
{
	sem_wait(&(data->nopal));
	FILE* o = fopen("nopalin.out", "a");
	fprintf(o, "%i\t%i\t%s", getpid(), pos, data->rows[pos]);
	fclose(o);
	sem_post(&(data->nopal));
}

int PalinCheck(int pos)
{
	int len = strlen(data->rows[pos]) - 2;
	int left = 0;

	while (len > left)
	{
		//printf("\nComparing %i to %i %c to %c\n", len, left, data->rows[pos][len], data->rows[pos][left]);
		if (data->rows[pos][len--] != data->rows[pos][left++])
		{
			//printf("FAIL %s", data->rows[pos]);
			return 0;
		}

	}
	return 1;
}

void ShmAttatch()
{
	key_t shmkey = ftok("shmshare", 765); //shared mem key

	if (shmkey == -1) //check if the input file exists
	{
		printf("\n%s: ", filen);
		fflush(stdout);
		perror("Error: Ftok failed");
		return;
	}

	ipcid = shmget(shmkey, sizeof(Shared), 0600 | IPC_CREAT); //get shared mem

	if (ipcid == -1) //check if the input file exists
	{
		printf("\n%s: ", filen);
		fflush(stdout);
		perror("Error: failed to get shared memory");
		return;
	}

	data = (Shared*)shmat(ipcid, (void*)0, 0); //attach to shared mem

	if (data == (void*)-1) //check if the input file exists
	{
		printf("\n%s: ", filen);
		fflush(stdout);
		perror("Error: Failed to attach to shared memory");
		return;
	}
}

int main(int argc, char** argv)
{
	filen = argv[0];
	ShmAttatch();

	int i;
	for (i = atoi(argv[1]); i < atoi(argv[1]) + 5; i++)
	{
		if (data->rowcount < i + 1)
			SetExit();

		if (PalinCheck(i))
			WritePalin(i);
		else
			WriteNonPalin(i);
	}

	SetExit();
	return 0;
}
