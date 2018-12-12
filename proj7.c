/**
Class: CPSC 346-02
Team Member 1: Quin Bingham
GU Username of project lead: qbingham
Pgm Name: proj7.c 
Pgm Desc: Read write problem with linux pipes and pthreads written in C.
*/


/**
TO COMPILE: 
gcc -lpthread proj7.c
./a.out 50
(50 will be the number of random numbers generated)
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>

typedef struct
{
 int numPrimes;
 int* pipefd;
 int num;
} params;

void *writer(void *);
void *reader(void *);
int isPrime(int);
int randInt(int, int);

int main(int argc, char *argv[])
{
	int numPrimes = atoi(argv[1]);

	int sleepTime, writeNum, readNum;

	int pipefd[2];
	pipe(pipefd);

	/*set arguments */
	sleepTime = 2;
	writeNum = 1;
	readNum = 3;

	/* Instantiate the writer and reader thread arrays */
	pthread_t writeThreads[writeNum];
	pthread_t readThreads[readNum];

	/* Create the writer and reader threads */
	for(int i = 0; i < writeNum; i++)
	{
		params p;
		p.numPrimes = numPrimes;
		p.num = i;
		p.pipefd = pipefd;
		pthread_create(&writeThreads[i], NULL, writer, (void*)&p);
	}

	for(int j = 0; j < readNum; j++)
	{
		params* p = malloc(sizeof *p);
		p->numPrimes = numPrimes;
		p->num = j;
		p->pipefd = pipefd;
		pthread_create(&readThreads[j], NULL, reader, (void*) p);
	}

	/*join on each thread*/
	for(int x = 0; x < writeNum; x++)
	{
		pthread_join(writeThreads[x],NULL);
	}

	for(int y = 0; y < readNum; y++)
	{
		pthread_join(readThreads[y],NULL);
	}

	return 0;
}

void *writer(void *params_in)
{
	params* p = (params*) params_in;

	int* pipefd = p->pipefd;

	for(int i = 0; i < p->numPrimes; i++)
	{
		int randomNumber = randInt(100,10000);

		printf("Generated: 	%i\n", randomNumber);
		write(pipefd[1], &randomNumber, sizeof(randomNumber));
	}

	close(pipefd[1]);
	pthread_exit(NULL);
}

void *reader(void *params_in)
{
	params* p = params_in;
	int* pipefd = p->pipefd;
	int buf;

	while (read(pipefd[0], &buf, sizeof(int)) > 0)
	{
		usleep(randInt(0,1000) * 1000);
		if (isPrime(buf))
        	printf("Reader %d: 	%i is prime\n", p->num, buf);
    }

	free(p);
	pthread_exit(NULL);
}

int isPrime(int prime)
{
    int sqrtPrime = (int) sqrt(prime);
    int isPrime = 1;

    for(int i = 2; i<sqrtPrime; i++)
    {
        if(prime%i == 0)
        {
            isPrime = 0;
            break;
        }
    }
    return isPrime;
}

int randInt(int min, int max)
{
	return rand() % (max +1 - min) + min;
}