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

/* Testeo
void estaVacio(void){
	if(queue_empty() == 1){
	printf("The queue is empty\n");

}
	else if(queue_full() == 1){
	printf("The queue is full\n");
	}
	else 	printf("The queue is a la mitad\n");
}
*/
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
	/* Testeo
	queue_init(4);
	estaVacio();
	printf("\n");
	struct element test1 = {-1,1,-1};
	queue_put(&test1);
	printf("\n");
	estaVacio();
	struct element test2 = {-1,2,-1};
	queue_put(&test2);
	printf("\n");
	estaVacio();
	struct element test3 = {-1,3,-1};
	queue_put(&test3);
	printf("\n");
	estaVacio();
	struct element test4 = {-1,4,-1};
	queue_put(&test4);
	printf("\n");
	estaVacio();
	struct element *viene;
	viene =  queue_get();
	printf("\n");
	int id = viene -> id_belt;
	int num = viene -> num_edition;
	int ultimo = viene -> last;
	printf("\tEn el process\n");
	printf("Id %i of the queue\n",id);
	printf("Num %i of the queue\n",num);
	printf("Ultimo %i of the queue\n",ultimo);
	estaVacio();
	printf("Destroy the queue\n");
	queue_destroy();*/
        printf("[OK][process_manager] Belt with id: %s has been created with a maximum of %s elements.\n",argv[1],argv[4]);

        if(sem_close(sem_process)<0) {
                printf("[ERROR][process_manager] There was an error executing process_manager with id: %s.\n",argv[1]);
                return -1;

        }
        return 0;
}
