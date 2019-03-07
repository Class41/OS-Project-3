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
Shared* data;
char rows[500][80];
int rowcount = -1;
char* filen;

void DoFork(int value) //do fun fork stuff here. I know, very useful comment.
{
	char* convert[15];
	sprintf(convert, "%i", value); //convert int to char in the most inneficient way possible 
	char* forkarg[] = {
			"./palin",
			convert,
			NULL
	}; //null terminated parameter array of chars

	execv(forkarg[0], forkarg); //exec
	printf("Exec failed! Aborting."); //all is lost. we couldn't fork. Blast.
	handler(1);
}

void ShmAttatch()
{
	key_t shmkey = ftok("shmshare", 765); //shared mem key

	if (shmkey == -1) //check if the input file exists
	{
		printf("\n%s: ", filename);
		fflush(stdout);
		perror("Error: Ftok failed");
		return;
	}

	ipcid = shmget(shmkey, sizeof(Shared), 0600 | IPC_CREAT); //get shared mem

	if (ipcid == -1) //check if the input file exists
	{
		printf("\n%s: ", filename);
		fflush(stdout);
		perror("Error: failed to get shared memory");
		return;
	}

	data = (Shared*)shmat(ipcid, (void*)0, 0); //attach to shared mem

	if (data == (void*)-1) //check if the input file exists
	{
		printf("\n%s: ", filename);
		fflush(stdout);
		perror("Error: Failed to attach to shared memory");
		return;
	}

	data->rows = new char[500][80];
}

void DoSharedWork(int childMax) //This is where the magic happens. Forking, and execs be here
{
	outfilename = output; //global outpt filename
	numpids = childMax;  //global pid count
	cPids = calloc(rowcount, sizeof(int)); //dynamically allocate a array of pids

	int status; //keeps track of status of waited pids
	//signal(SIGQUIT, handler); //Pull keycombos
	//signal(SIGINT, handler); //pull keycombos
	int i; //generic iterator. I call him bob.
	int remainingExecs = rowcount; 
	int activeExecs = 0; //how many execs are going right now
	int exitcount = 0; //how many exits we got
	int cPidsPos = 0; //wher we are in the cpid array
	int currentRowLine = 0;
	//FILE* o = fopen(output, "a"); //open the output file


	while (1) {
		pid_t pid; //pid temp
		int usertracker = -1; //updated by userready to the position of ready struct to be launched
		if (activeExecs < 19 && remainingExecs > 0)
		{
			pid = fork(); //the mircle of proccess creation

			if (pid < 0) //...or maybe not proccess creation if this executes
			{
				perror("Failed to fork, exiting");
				handler(1);
			}

			remainingExecs--; //we have less execs now since we launched successfully
			if (pid == 0)
			{
				DoFork(currentRowLine, output); //do the fork thing with exec followup
			}
			
			printf(o, "%s: PARENT: STARTING CHILD %i WITH PARAM %i\n", filen, pid, currentRowLine); //we are parent. We have made child with this value
			cPids[cPidsPos] = pid; //add pid to pidlist
			cPidsPos++; //increment pid list
			activeExecs++; //increment active execs
			currentRowLine++;
		}

		if ((pid = waitpid((pid_t)-1, &status, WNOHANG)) > 0) //if a PID is returned
		{
			if (WIFEXITED(status))
			{
				if (WEXITSTATUS(status) == 21) //21 is my custom return val
				{
					exitcount++;
					activeExecs--;
					printf("%s: CHILD PID: %i: RIP. fun while it lasted: %i sec %i nano.\n", filen, pid, data->seconds, data->nanoseconds);
				}
			}
		}

		if (exitcount == rowcount && remainingExecs == 0) //only get out of loop if we run out of execs or we have maxed out child count
			break;
	}

	printf("((REMAINING: %i)))\n", remainingExecs);

	free(cPids); //free up memory
	shmdt(data); //detatch from shared mem
	shmctl(ipcid, IPC_RMID, NULL); //clear shared mem
	exit(0);
}

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
	ShmAttatch(); //attach to shared mem
		
	DoSharedWork(19); //do fork/exec fun stuff (20-1 for parent)

	return 0;
}
