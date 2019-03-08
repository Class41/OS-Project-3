//Vasyl Onufriyev
//3-6-19
//OS Spring 2019
//Project 3: Palindromes
//Purpose: Checks for palindromes

#include <stdio.h>
#include <stdlib.h>
#include "shared.h"
#include "semaphore.h"
#include <sys/ipc.h> 
#include <sys/shm.h> 

int ipcid;
Shared* data;


void SetExit()
{
	shmdt(data);
	exit(21);
}

void WritePalin(int pos)
{
	FILE* o = fopen("palin.out", "a");
	fprintf(o, "%i\t%i\t%s", getpid(), pos, data->rows[pos]);
	fclose(o);
}

void WriteNonPalin(int pos)
{
	FILE* o = fopen("nopalin.out", "a");
	fprintf(o, "%i\t%i\t%s", getpid(), pos, data->rows[pos]);
	fclose(o);
}

int PalinCheck(int pos)
{
	int len = 0;

	while (data->rows[pos][len] != '\0')
	{
		len++;
	}

	len--;

	int l;

	while (l < len)
	{
		if (data->rows[pos][len--] != data->rows[pos][l++])
		{
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
	ShmAttatch();

	int i;
	for (i = 0; i < 5; i++)
	{
		if (data->rowcount + i < atoi(argv[1]))
			SetExit();

		if (PalinCheck(i))
			WritePalin(i);
		else
			WriteNonPalin(i);
	}




	printf("Running! %s\n", argv[1]);

	SetExit();
	return 0;
}
