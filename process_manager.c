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
#include <signal.h>

//Thread function
void *PrintHello(void *threadid){
        long tid;
        tid = (long)threadid;
        printf("Hello World! It's me, thread #%ld!\n", tid);
        printf("Thread #%ld ends\n", tid);
        pthread_exit(0);
}

void estaVacio(void){
	if(queue_empty() == 1){
	printf("The queue is empty\n");

}
	else 	printf("The queue is not empty\n");
}
int main (int argc, const char * argv[] ){

        sem_t *sem_process;

        if(argc!=5) {
                printf("[ERROR][process_manager] Arguments not valid.\n");
                return -1;
        }

        /* Opening the named semaphore*/
        if((sem_process = sem_open(argv[2],O_CREAT,0644,0))==SEM_FAILED) {
                printf("[ERROR][process_manager] There was an error executing process_manager with id: %s.\n",argv[1]); /* Error message */
                return -1;
        }
        printf("[OK][process_manager] Process_manager with id: %s waiting to produce %s elements.\n",argv[1],argv[4]);

        if(sem_wait(sem_process) < 0) {
                printf("[ERROR][process_manager] There was an error executing process_manager with id: %s.\n",argv[1]);
                return -1;
        }
	printf("Create the queue\n");
	queue_init(4);
	estaVacio();
	struct element test = {5,atoi(argv[1]),0};
	printf("Insert 5 in the queue\n");
	queue_put(&test);
	estaVacio();
	struct element test1 = {7,atoi(argv[1]),0};
	printf("Insert 7 in the queue\n");
	queue_put(&test1);
	struct element *viene;
	viene =  queue_get();
	printf("Returnea %i in the queue\n",viene -> id_belt);
	estaVacio();
	printf("Destroy the queue\n");
	queue_destroy();
        printf("[OK][process_manager] Belt with id: %s has been created with a maximum of %s elements.\n",argv[1],argv[4]);

        if(sem_close(sem_process)<0) {
                printf("[ERROR][process_manager] There was an error executing process_manager with id: %s.\n",argv[1]);
                return -1;

        }
        return 0;
}
