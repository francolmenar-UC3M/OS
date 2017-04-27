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

pthread_mutex_t mutex;
pthread_cond_t no_full, no_empty;
int elemTo;
int idBelt;

void *producer_exec(void* param){
	
	int i;
	for (i=0; i<elemTo; i++){
		//struct element* elem;
		//elem->id_belt=idBelt;
		
struct element elem={i,idBelt,-1};

		if(pthread_mutex_lock(&mutex)!=0){
		printf("[ERROR][queue] There was an error while using queue with id: %i.\n",idBelt); /* Error message */
		}
		
		while(queue_full()==1){
			if(pthread_cond_wait(&no_full,&mutex)!=0){
			printf("[ERROR][queue] There was an error while using queue with id: %i.\n",idBelt); /* Error message */
			}
		}
		
		if(queue_put(&elem)<0){
		printf("[ERROR][queue] There was an error while using queue with id: %i.\n",idBelt); /* Error message */
		}
		printf("[OK][queue] Introduced element with id: %i in belt %i\n",i,idBelt);
	
		if(pthread_cond_signal(&no_empty)!=0){
		printf("[ERROR][queue] There was an error while using queue with id: %i.\n",idBelt); /* Error message */
		}
		if(pthread_mutex_unlock(&mutex)!=0){
		printf("[ERROR][queue] There was an error while using queue with id: %i.\n",idBelt); /* Error message */
		}
	
	}
	//printf("Producer: %i\n",queue_full());
	pthread_exit(0);
}
		
void *consumer_exec(void* param){
	
	//printf("Consumer: okidoki\n");
	
	int i;
	for (i=0; i<elemTo; i++){
		struct element* elem;
		
		if(pthread_mutex_lock(&mutex)!=0){
		printf("[ERROR][queue] There was an error while using queue with id: %i.\n",idBelt); /* Error message */
		}
	
		while(queue_empty()==1){
			if(pthread_cond_wait(&no_empty,&mutex)!=0){
			printf("[ERROR][queue] There was an error while using queue with id: %i.\n",idBelt); /* Error message */
			}
		}
		
		elem = queue_get();
		
		printf("[OK][queue] Obtained element with id: %i in belt %i\n",elem->num_edition,elem->id_belt);
	
		if(pthread_cond_signal(&no_full)!=0){
		printf("[ERROR][queue] There was an error while using queue with id: %i.\n",elem->id_belt); /* Error message */
		}
		if(pthread_mutex_unlock(&mutex)!=0){
		printf("[ERROR][queue] There was an error while using queue with id: %i.\n",elem->id_belt); /* Error message */
		}
	}
	pthread_exit(0);
}

int main (int argc, const char * argv[] ){

        sem_t *sem_process;
		pthread_t producer, consumer;
		
		int n_elements;

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
		
		queue_init(atoi(argv[3]));
		idBelt = atoi(argv[1]);
		elemTo = atoi(argv[4]);
		
		printf("[OK][process_manager] Belt with id: %s has been created with a maximum of %s elements.\n",argv[1],argv[3]);
		
		if(pthread_mutex_init(&mutex,NULL)!=0){
			printf("[ERROR][process_manager] There was an error executing process_manager with id: %s.\n",argv[1]); /* Error message */
			return -1;
		}
		
		if(pthread_cond_init(&no_full,NULL)!=0){
			printf("[ERROR][process_manager] There was an error executing process_manager with id: %s.\n",argv[1]); /* Error message */
			return -1;
		}
		
		if(pthread_cond_init(&no_empty,NULL)!=0){
			printf("[ERROR][process_manager] There was an error executing process_manager with id: %s.\n",argv[1]); /* Error message */
			return -1;
		}
		
		if(pthread_create(&producer,NULL,producer_exec,NULL)!=0){
			printf("[ERROR][process_manager] There was an error executing process_manager with id: %s.\n",argv[1]); /* Error message */
			return -1;
		}
		
		if(pthread_create(&consumer,NULL,consumer_exec,NULL)!=0){
			printf("[ERROR][process_manager] There was an error executing process_manager with id: %s.\n",argv[1]); /* Error message */
			return -1;
		}
		
		if(pthread_join(producer,NULL)!=0){
			printf("[ERROR][process_manager] There was an error executing process_manager with id: %s.\n",argv[1]); /* Error message */
			return -1;
		}
		
		if(pthread_join(consumer,NULL)!=0){
			printf("[ERROR][process_manager] There was an error executing process_manager with id: %s.\n",argv[1]); /* Error message */
			return -1;
		}
		
		queue_destroy();
		
		if(pthread_mutex_destroy(&mutex)!=0){
			printf("[ERROR][process_manager] There was an error executing process_manager with id: %s.\n",argv[1]); /* Error message */
			return -1;
		}
		
		if(pthread_cond_destroy(&no_full)!=0){
			printf("[ERROR][process_manager] There was an error executing process_manager with id: %s.\n",argv[1]); /* Error message */
			return -1;
		}
		
		if(pthread_cond_destroy(&no_empty)!=0){
			printf("[ERROR][process_manager] There was an error executing process_manager with id: %s.\n",argv[1]); /* Error message */
			return -1;
		}
		
		printf("[OK][process_manager] Process_manager with id: %s has produced %s elements.\n",argv[1],argv[4]);
		
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

        if(sem_close(sem_process)<0) {
                printf("[ERROR][process_manager] There was an error executing process_manager with id: %s.\n",argv[1]);
                return -1;

        }
        return 0;
}
