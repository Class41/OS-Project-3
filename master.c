//Vasyl Onufriyev
//3-6-19
//OS Spring 2019
//Project 3: Palindromes
//Purpose: Launch worker proccesses

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <errno.h>
#include "shared.h"
#include <signal.h>
#include <sys/time.h>
#include "string.h"

int cPids[19]; //list of pids
int ipcid; //inter proccess shared memory
char rows[500][80];
int rowcount = -1;
char* filen;

int parsefile(FILE* in) //reads in input file and parses input
{
	char line[80];

	printf("%s: PARENT: BEGIN FILE PARSE\n", filen);
	while (!feof(in)) //keep reading until end of line
	{
		rowcount++;
		if (rowcount > 500)
		{
			printf("%s: PARENT: TOO MANY LINES IN FILE. MAX 500", filen);
			return 1;
		}

		fgets(line, 80, in);
		
		memcpy(&(rows[rowcount][0]), line, 80);
	}
	
	int i;
	for (i = 0; i < rowcount; i++) //output parse data
	{
		printf("%s: PARENT: PARSED: %s\n", filen, rows[i]);
		fflush(stdout);
	}
}

void timerhandler(int sig) //2 second kill timer
{
	handler(sig);
}

int setupinterrupt() //setup interrupt handling
{
	struct sigaction act;
	act.sa_handler = timerhandler;
	act.sa_flags = 0;
	return (sigemptyset(&act.sa_mask) || sigaction(SIGPROF, &act, NULL));
}

int setuptimer() //setup timer handling
{
	struct itimerval value;
	value.it_interval.tv_sec = 2;
	value.it_interval.tv_usec = 0;
	value.it_value = value.it_interval;
	return (setitimer(ITIMER_PROF, &value, NULL));
}

void handler(int signal) //handle ctrl-c and timer hit
{
	printf("%s: Kill Signal Caught. Killing children and terminating...", filen);
	fflush(stdout);

	int i;
	for (i = 0; i < 19; i++)
	{
		if (cPids[i] > 0) //kill all pids just in case
		{
			kill(cPids[i], SIGTERM);
		}
	}

	free(cPids); //free used memory
	shmctl(ipcid, IPC_RMID, NULL); //free shared mem


	kill(getpid(), SIGTERM); //kill self
}



int main(int argc, char** argv)
{
	if (setupinterrupt() == -1) //handler for SIGPROF failed
	{
		perror("Failed to setup handler for SIGPROF");
		return 1;
	}
	if (setuptimer() == -1) //timer failed
	{
		perror("Failed to setup ITIMER_PROF interval timer");
		return 1;
	}

	filen = argv[0]; //shorthand for filename
	int optionItem; 
	int childMax = 4; //default
	int childConcurMax = 2; //default

	FILE* input = fopen("palin.in", "r"); //open input/output files specified
	FILE* output = fopen("palin.out", "wr");
	fclose(output);
	output = fopen("nopalin.out", "wr");
	fclose(output);

	if (input == NULL) //check if the input file exists
	{
		printf("\n%s: ", argv[0]);
		fflush(stdout);
		perror("Error: Failed to open input file");
		return;
	}

	parsefile(input); //read file contents		
		
	//DoSharedWork(argv[0], 19); //do fork/exec fun stuff (20-1 for parent)

	return 0;
}
