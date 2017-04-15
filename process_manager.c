/*
 *
 * process_manager.c
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stddef.h>
#include <pthread.h>
#include "queue.h"
#include <semaphore.h>


#define NUM_THREADS 2

//Thread function
void *PrintHello(void *threadid){
   	long tid;
   	tid = (long)threadid;
   	printf("Hello World! It's me, thread #%ld!\n", tid);
	printf("Thread #%ld ends\n", tid);
   	pthread_exit(0);
}


int main (int argc, const char * argv[] ){
  
	sem_t *sem_process;
	/* Check the correct input of the parameters */
	if(argc!=5){
		printf("[ERROR][process_manager] Arguments not valid.\n");
		return -1;
	}
	/* Opening the named semaphore*/
	if((sem_process = sem_open(argv[2],O_CREAT,0644,0))==SEM_FAILED){
		printf("[ERROR][process_manager] There was an error executing process_manager with id: %s.\n",argv[1]);  /* Error message */
  		return -1;
    }
	//sem_post(sem_process);
	/* Wait for the signal of the factory manager to start to produce*/
	if (sem_wait(sem_process)<0) {
        printf("[ERROR][process_manager] There was an error executing process_manager with id: %s.\n",argv[1]);
		return -1;
	}
		printf("[OK][process_manager] Process_manager with id: %s waiting to produce %s elements.\n",argv[1],argv[4]);
	if(sem_post(sem_process) < 0){
        printf("[ERROR][process_manager] There was an error executing process_manager with id: %s.\n",argv[1]);
				return -1;
	}
	if (sem_wait(sem_process)<0) {
        printf("[ERROR][process_manager] There was an error executing process_manager with id: %s.\n",argv[1]);
		return -1;
	}
	//sem_post(sem_process);
	printf("[OK][process_manager] Process_manager with id: %s have produced %s elements.\n",argv[1],argv[4]);
    if(sem_post(sem_process) < 0){
        printf("[ERROR][process_manager] There was an error executing process_manager with id: %s.\n",argv[1]);
				return -1;
	}
	return 0;
}
