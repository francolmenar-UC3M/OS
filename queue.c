#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include "queue.h"

/* Declare the variables */
struct element *circular_buffer; /* We have defined the queue as a circular buffer */
int number_of_elements;          /* Indicates the current number of elements in the belt */
int max_capacity;                /* Indicates the maximum number of elements that can be currently at the same time in the belt */
int counter = 0;                 /* counter is used to assign num_edition */
/*
	Function that creates the queue and reserves memory for the size specified as parameter.
	Always returns 0.
*/
int queue_init(int size){
	circular_buffer = malloc((sizeof(struct element)) * size); /* Assign the specified memory by size --> struct element size times */
	number_of_elements = 0;                                    /* The queue is empty at the beginning */
	max_capacity = size;                                       /* Set the maximum capacity of the queue to size */
	return 0;
}


/* Function that puts the element passed as parameter, x, in the queue. Used by the thread producer in process_manager. 
   Returns -1 only if the structure is full. Else, returns 0. 
*/
int queue_put(struct element* x) {
	/* Three cases:
	· The queue is full --> impossible to put an element. Return -1.
	· The queue is empty --> put the first element. Return 0.
	· The queue is not full nor empty --> trasverse the queue to put the element. Returns 0.
	*/
	if(queue_full() == 1){         /* Case: queue is full */
		return -1;                 /* Unique case impossible to put an element */
	}
	else if(queue_empty() == 1){   /* Case: queue is empty */
		x -> last = 1;             /* Now x is the last element in the queue (only one element in the queue) */
		x -> num_edition = counter;/* Its num_edition will be always 0 (first element) */
		circular_buffer[0] = *x;   /* The element is put in the first position of the buffer */
		number_of_elements++;      /* There is one element more in the queue */
		counter++;                 /* Increase the counter for the next element */
		return 0;
	}
	int i;
	for(i = 0; i < max_capacity; i++){    /* Trasverse the queue up to max capacity to reach last element */
		if(circular_buffer[i].last == 1){ /* In case we find the last element of the queue */ 
			circular_buffer[i].last = 0;  /* That element is no more the last element */
			x -> num_edition = counter;   /* Assign the ID of the element (number of elements introduced -1)*/
			x -> last = 1;                /* Now this element is the last */
			number_of_elements++;         /* There is one element more in the queue */
			counter++;                    /* Increase the counter for the next element */
			/* Insert the element into the queue. Use %max_capacity as the buffer is circular 
			(when reached the maximum capacity start inserting at the beginning) */
			circular_buffer [(i+1) % max_capacity] = *x; 
			return 0;
		}
	}
}


/* Function that gets an element from the queue. Used by the thread consumer in process_manager. 
   Returns the obtained element. 
*/
struct element* queue_get(void) {
	/* Allocate memory for the auxiliary element used in this method. */
	struct element* aux = malloc(sizeof(struct element));
	int i;
	for(i = 0; i < max_capacity; i++){       /* Go through all the queue */
		if(circular_buffer[i].last == 1){    /* In case we have reached the last element of the belt, get it */
			/* aux is now in the place of the retrieved element (an empty element)*/
			*aux = circular_buffer [((i+1)-number_of_elements) % max_capacity];
		}
	}
	number_of_elements--;                    /* There is one element less in the queue */
	return aux;
}


/*
	Function that checks if the belt is empty or not. Used by the thread consumer in process_manager
	Returns 1 if the queue is empty and 0 if not.
*/
int queue_empty(void){
	if (number_of_elements == 0) return 1; /* We have to check only if the number of elements is 0 or not */
	else return 0;
}

/*
	Function that checks if the belt is full or not. Used by the thread producer in process_manager
	Returns 1 if the queue is full and 0 if not.
*/
int queue_full(void){
	if (number_of_elements == max_capacity) return 1; /* We have to check only if the number of elements is equal to the maximum allowed */
	else return 0;
}

/* In order to free resources, we destroy the queue --> free the memory allocated to the circular buffer */
int queue_destroy(void){
	free(circular_buffer);
	return 0;
}
