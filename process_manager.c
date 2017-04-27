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

/* Declare global variables. They must be initialized here to work on upper methods. */
pthread_mutex_t mutex;            /* Initialize the mutex */
pthread_cond_t no_full, no_empty; /* Initialize the conditions for the mutex */
int elemTo;                       /* Number of elements to be created*/
int numEdition;                   /* id of the element inside the queue */
int idBelt;                       /* id of the belt */
int Last;                         /* Last element of the queue */

/* Method which executes the thread producer.
This thread is in charge of creating the number of elements specified by elemTo waiting if the queue is full.
This method returns -1 if there is an error, else, returns 0 */
void *producer_exec(void* param){
	
	int i;                      /* Counter */
	/* In each iteration, the producer puts an element in the queue. We have to put elemTo elements */
	for (i=0; i<elemTo; i++){
		
	/* elem {num_edition, id_belt, last} */
	struct element elem={numEdition,idBelt,Last}; /* Initialize the structure element. One different element per iteration */
	
		/* Every time the thread enters the producer_exec method, it must lock the mutex so that nobody else enters.*/
		if(pthread_mutex_lock(&mutex)!=0){
		fprintf(stderr,"[ERROR][queue] There was an error while using queue with id: %i.\n",idBelt); /* Error message */
		}
		
		/* The thread must wait for a consumer if the queue is full --> mutex unlocked */
		while(queue_full()==1){
			if(pthread_cond_wait(&no_full,&mutex)!=0){ /* Waits for the condition 'no_full' */
			fprintf(stderr,"[ERROR][queue] There was an error while using queue with id: %i.\n",idBelt); /* Error message */
			}
		}
		
		/* If the queue is not full, the producer can put an element in the queue */
		if(queue_put(&elem)<0){
		fprintf(stderr,"[ERROR][queue] There was an error while using queue with id: %i.\n",idBelt); /* Error message */
		}
		/* Notify that the element has been introduced */
		printf("[OK][queue] Introduced element with id: %i in belt %i\n",i,idBelt);
	
	    /* Signal the consumer threads that the queue is not empty --> at least one element inside */
		if(pthread_cond_signal(&no_empty)!=0){
		fprintf(stderr,"[ERROR][queue] There was an error while using queue with id: %i.\n",idBelt); /* Error message */
		}
		/* Now we can leave the mutex and unlock it */
		if(pthread_mutex_unlock(&mutex)!=0){
		fprintf(stderr,"[ERROR][queue] There was an error while using queue with id: %i.\n",idBelt); /* Error message */
		}
	
	}
	/* The method exists the thread returning 0 --> success */
	pthread_exit(0);
}

/* Method which executes the thread consumer.
This thread is in charge of getting the number of elements specified by elemTo waiting if the queue is empty.
This method returns -1 if there is an error, else, returns 0 */		
void *consumer_exec(void* param){
	
	int i;             /* Counter */
	/* In each iteration, the producer gets an element from the queue. We have to get elemTo elements */
	for (i=0; i<elemTo; i++){
		/* There's no need to initialize every element, as we are going to get it from queue_get() in each iteration */
		struct element* elem;  
		
		/* Every time the thread enters the consumer_exec method, it must lock the mutex so that nobody else enters.*/
		if(pthread_mutex_lock(&mutex)!=0){
		fprintf(stderr,"[ERROR][queue] There was an error while using queue with id: %i.\n",idBelt); /* Error message */
		}
		
		/* The thread must wait for a producer if the queue is empty --> mutex unlocked */
		while(queue_empty()==1){
			if(pthread_cond_wait(&no_empty,&mutex)!=0){/* Waits for the condition 'no_empty' */
			fprintf(stderr,"[ERROR][queue] There was an error while using queue with id: %i.\n",idBelt); /* Error message */
			}
		}
		/* If the queue is not empty, the consumer can get an element from the queue */
		elem = queue_get();
		/* Notify that the element has been obtained */
		printf("[OK][queue] Obtained element with id: %i in belt %i\n",i,idBelt);
		
		/* Signal the producer threads that the queue is not full --> at least one hole inside */
		if(pthread_cond_signal(&no_full)!=0){
		fprintf(stderr,"[ERROR][queue] There was an error while using queue with id: %i.\n",idBelt); /* Error message */
		}
		
		/* Now we can leave the mutex and unlock it */
		if(pthread_mutex_unlock(&mutex)!=0){
		fprintf(stderr,"[ERROR][queue] There was an error while using queue with id: %i.\n",idBelt); /* Error message */
		}
	}
	/* The method exists the thread returning 0 --> success */
	pthread_exit(0);
}

/* This class receives the needed arguments for the execution of a child process created by factory_manager.
Returns 0 if correct execution, else, returns -1. */
int main (int argc, const char * argv[] ){
		
        sem_t *sem_process;           /* Declare the named semaphore from factory_manager */
		pthread_t producer, consumer; /* Declare both threads: producer and consumer */
		
		/* The number of arguments passed from factory_manager must be 5*/
        if(argc!=5) {
            fprintf(stderr,"[ERROR][process_manager] Arguments not valid.\n");
            return -1;
        }

        /* Opening the named semaphore, the one with the name passed as parameter in argv[2]
		--> created by the father specifficaly for this child */
        if((sem_process = sem_open(argv[2],O_CREAT,0644,0))==SEM_FAILED) {
            fprintf(stderr,"[ERROR][process_manager] There was an error executing process_manager with id: %s.\n",argv[1]); /* Error message */
            return -1;
        }
		/* Show process is waiting, after being created in factory_manager */
        printf("[OK][process_manager] Process_manager with id: %s waiting to produce %s elements.\n",argv[1],argv[4]);
		
		/* Process waits until all the children has been created */
        if(sem_wait(sem_process) < 0) {
            fprintf(stderr,"[ERROR][process_manager] There was an error executing process_manager with id: %s.\n",argv[1]); /* Error message */
            return -1;
        }
		/*Now, the process is ready to be executed. Therefore, initialize the queue using the maximum number of elements for the belt (argv[3]) */
		queue_init(atoi(argv[3]));
		/* idBelt and elemTo, used by the threads, are initialized using the corresponding arguments of the process */
		idBelt = atoi(argv[1]); /* idBelt --> ID of the process */
		elemTo = atoi(argv[4]); /* elemTo --> number of elements to create */
		
		/* Notify that the process has been created */
		printf("[OK][process_manager] Belt with id: %s has been created with a maximum of %s elements.\n",argv[1],argv[3]);
		
		/* Initialize all the variables for the synchronization of the threads: 
		-->mutex
		-->cond: no_full
		-->cond: no_empty 
		*/
		
		if(pthread_mutex_init(&mutex,NULL)!=0){ /* Mutex */
			fprintf(stderr,"[ERROR][process_manager] There was an error executing process_manager with id: %s.\n",argv[1]); /* Error message */
			return -1;
		}
		
		if(pthread_cond_init(&no_full,NULL)!=0){ /* Condition no full */
			fprintf(stderr,"[ERROR][process_manager] There was an error executing process_manager with id: %s.\n",argv[1]); /* Error message */
			return -1;
		}
		
		if(pthread_cond_init(&no_empty,NULL)!=0){ /* Condition no empty */
			fprintf(stderr,"[ERROR][process_manager] There was an error executing process_manager with id: %s.\n",argv[1]); /* Error message */
			return -1;
		}
		
		/* Create and execute the method reserved for both threads:
		producer executes producer_exec
		consumer executes consumer_exec
		
		In this execution (see above) is were both threads must be synchonized */
		if(pthread_create(&producer,NULL,producer_exec,NULL)!=0){ /* Producer */
			fprintf(stderr,"[ERROR][process_manager] There was an error executing process_manager with id: %s.\n",argv[1]); /* Error message */
			return -1;
		}
		
		if(pthread_create(&consumer,NULL,consumer_exec,NULL)!=0){ /* Consumer */
			fprintf(stderr,"[ERROR][process_manager] There was an error executing process_manager with id: %s.\n",argv[1]); /* Error message */
			return -1;
		}
		
		/* Join the threads (producer and consumer) so that its process waits for them to end execution (when reaching pthread_exit(0))
		
		Process is stopped here until complete execution of threads*/
		if(pthread_join(producer,NULL)!=0){ /* Wait producer */
			fprintf(stderr,"[ERROR][process_manager] There was an error executing process_manager with id: %s.\n",argv[1]); /* Error message */
			return -1;
		}
		
		if(pthread_join(consumer,NULL)!=0){ /* And wait also the consumer */
			fprintf(stderr,"[ERROR][process_manager] There was an error executing process_manager with id: %s.\n",argv[1]); /* Error message */
			return -1;
		}
		
		/* After the execution of the threads we don't need the queue anymore --> Destroy the queue for freeing resources */
		queue_destroy();
		
		/* Free all the remaining resources created during the execution of process_manager:
		-->mutex
		-->cond: no_full
		-->cond: no_empty 
		*/
		if(pthread_mutex_destroy(&mutex)!=0){ /* Mutex */
			fprintf(stderr,"[ERROR][process_manager] There was an error executing process_manager with id: %s.\n",argv[1]); /* Error message */
			return -1;
		}
		
		if(pthread_cond_destroy(&no_full)!=0){ /* Condition no full */
			fprintf(stderr,"[ERROR][process_manager] There was an error executing process_manager with id: %s.\n",argv[1]); /* Error message */
			return -1;
		}
		
		if(pthread_cond_destroy(&no_empty)!=0){ /* Condition no empty */
			fprintf(stderr,"[ERROR][process_manager] There was an error executing process_manager with id: %s.\n",argv[1]); /* Error message */
			return -1;
		}
		
		/* After terminating the execution of the queue and freeing all the resources, we just have to notify the elements that have been produced */
		printf("[OK][process_manager] Process_manager with id: %s has produced %s elements.\n",argv[1],argv[4]);
		
		/* Finally, close the named semaphore of this process. It will be unlinked in factory_manager */
        if(sem_close(sem_process)<0) {
                printf("[ERROR][process_manager] There was an error executing process_manager with id: %s.\n",argv[1]);
                return -1;

        }
        return 0;
}
