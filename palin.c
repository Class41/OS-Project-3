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

int ipcid; //shared memory id
Shared* data; //shared memory data
char* filen; //this filename


void SetExit() //universal exit function
{
	shmdt(data);
	exit(21);
}

void WritePalin(int pos) //if palindrome, this function will be called
{
	fprintf(stderr, "[ENTER -> pid %i] [%i] palindrome section.\n", getpid(), time(NULL));
	sem_wait(&(data->pal)); //lock out any other proccess from entering the critical section
	sleep(rand() % 3); //sleep from 0 - 3 sec
	FILE* o = fopen("palin.out", "a"); //open file
	fprintf(o, "%i\t%i\t%s", getpid(), pos, data->rows[pos]); //write palin to file
	fclose(o); //close file
	sleep(rand() % 3); //sleep from 0 - 3 sec
	fprintf(stderr, "[EXIT -> pid %i] [%i] palindrome section\n", getpid(), time(NULL));
	sem_post(&(data->pal));
}

void WriteNonPalin(int pos) //if non-palindrome, this function will be called
{
	fprintf(stderr, "[ENTER -> pid %i] [%i] non-palindrome section.\n", getpid(), time(NULL));
	sem_wait(&(data->nopal)); //lock out any other proccess from entering the critical section
	sleep(rand() % 3); //sleep from 0 - 3 sec
	FILE* o = fopen("nopalin.out", "a"); //open file
	fprintf(o, "%i\t%i\t%s", getpid(), pos, data->rows[pos]); //write nonpalin to file
	fclose(o); //close file
	sleep(rand() % 3); //sleep for 0 - 3 sec
	fprintf(stderr, "[EXIT -> pid %i] [%i] non-palindrome section.\n", getpid(), time(NULL));
	sem_post(&(data->nopal)); //release critical section
}

int PalinCheck(int pos)
{
	int len = strlen(data->rows[pos]) - 2; //determine length of string need to account for \0 and 0 position of array.
	int left = 0; //left side of the word

	while (len > left) //keep going until we end up on the same character
	{
		//printf("\nComparing %i to %i %c to %c\n", len, left, data->rows[pos][len], data->rows[pos][left]);
		if (data->rows[pos][len--] != data->rows[pos][left++]) //converge left and right sides in the middle of the word. If at any time they don't match, return 0
		{
			//printf("FAIL %s", data->rows[pos]);
			return 0;
		}

	}
	return 1; //word mirrored fine, so it is a palin
}

void ShmAttatch() //same exact memory attach function from master minus the init for the semaphores
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
	srand(time(0)); //random seed
	filen = argv[0]; //set filename globally
	ShmAttatch(); //attach to shared memory

	int i; 
	for (i = atoi(argv[1]); i < atoi(argv[1]) + 5; i++) 
	{
		if (data->rowcount < i + 1) //we have exceeded the max length of the array. Exit
			SetExit();

		if (PalinCheck(i)) //check if this is a palindrome
			WritePalin(i);
		else
			WriteNonPalin(i);
	}

	SetExit(); //exit application
	return 0;
}
