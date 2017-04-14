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
  printf("Its process_manager\n");
  sem_t *sem_process = sem_open(argv[2], O_RDWR);
  if((sem_process) == SEM_FAILED){
	  printf("Error");
	  return -1;
  }/*  LOOP INFINITO
  int i;
    for (i = 0; i < 10; i++) {
        if (sem_wait(sem_process) < 0) {
            perror("sem_wait(3) failed on child");
            continue;
        }

        printf("PID %ld acquired semaphore\n", (long) getpid());

        if (sem_post(sem_process) < 0) {
            perror("sem_post(3) error on child");
        }

        sleep(1);
    }
  if (sem_close(sem_process) < 0)
        perror("sem_close(3) failed");
*/
    return 0;
}
