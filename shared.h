#ifndef SHARED_H

#include <semaphore.h>

typedef struct {
	char rows[500][80];
	int rowcount;
	sem_t pal;
	sem_t nopal;
} Shared;

#define SHARED_H
#endif
