//Vasyl Onufriyev
//3-6-19
//OS Spring 2019
//Project 3: Palindromes
//Purpose: Checks for palindromes

#include <stdio.h>
#include <stdlib.h>
#include "shared.h"

int ipcid;
Shared* data;


void SetExit()
{
		shmdt(data);
		exit(21);
}

void WritePalin()
{

}

void WriteNonPalin()
{
	
}

int PalinCheck()
{
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
		if(data->rowcount + i < atoi(argv[1]))
			SetExit();
		
		if(PalinCheck())
			WritePalin();
		else
			WriteNonPalin();
	}


	

	printf("Running! %s\n", argv[1]);

	SetExit();
	return 0;
}
