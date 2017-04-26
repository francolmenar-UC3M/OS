#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include "queue.h" /* https://github.com/stockrt/queue.h/blob/master/sample.c */

struct element *circular_buffer; //It is the queue as an circular_buffer
int number_of_elements; //It is the number of elements currently in the queue
int max_capacity;//The maximum number of elements that the queue can have
int counter = 0;//To assign the num_edition

/*
	Function that creates the queue and reserves
	memory for the size specified as parameter.
*/
int queue_init(int size){
	circular_buffer = malloc((sizeof(struct element)) * size); //I assign the needed memory to the circular_buffer
	number_of_elements = 0;//The queue is initially empty
	max_capacity = size;//We set the maximun capacity of the queue
	return 0;
}


// To Enqueue an element
int queue_put(struct element* x) {
	if(queue_full() == 1){//Check if the queue is full
		return -1;//If it is full no inserting is done
	}
	else if(queue_empty() == 1){ //If the queue is empty
		x -> last = 1; //It is the last
		x -> num_edition = counter;
		circular_buffer[0] = *x; 
		number_of_elements++; //There is one element more in the queue
		counter++;
		return 0;
	}
	int i;
	for(i = 0; i < max_capacity; i++){
		if(circular_buffer[i].last == 1){ //It is the last element in the queue
			circular_buffer[i].last = 0; //It is no more the last element
			x -> num_edition = counter;
			x -> last = 1;
			number_of_elements++; //There is one element more in the queue
			counter++;
			circular_buffer [(i+1) % max_capacity] = *x; //I insert the element into the queue. It is the possition of the
			return 0;
		}
	}



}


// To Dequeue an element.
struct element* queue_get(void) {
	struct element* aux = malloc(sizeof(struct element));//We allocate the memory needed for the auxiliar element
	int i;
	for(i = 0; i < max_capacity; i++){ //I go through all the queue
		if(circular_buffer[i].last == 1){ //It is the last element in the queue
			*aux = circular_buffer [((i+1)-number_of_elements) % max_capacity];
		}
	}
	number_of_elements--;//There is one element less int the queue
	return aux;
}


/*
	function that checks the state of the queue and determines if it is
	empty (returns 1) or not (returns 0)
*/
int queue_empty(void){
	if (number_of_elements == 0) return 1;
	else return 0;
}

int queue_full(void){
	if (number_of_elements == max_capacity) return 1;
	else return 0;
}

//To destroy the queue and free the resources
int queue_destroy(void){
	free(circular_buffer);
	return 0;
}
