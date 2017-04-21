#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include "queue.h" /* https://github.com/stockrt/queue.h/blob/master/sample.c */

/*
	Function that creates the queue and reserves
	memory for the size specified as parameter.
*/
int queue_init(int size){
	return 0;
}


// To Enqueue an element
int queue_put(struct element* x) {
	return 0;
}


// To Dequeue an element.
struct element* queue_get(void) {
	return NULL;
}


/*
	function that checks the state of the queue and determines if it is
	empty (returns 1) or not (returns 0)
*/
int queue_empty(void){
	//if(circular_buffer -> count == 0) return 1;
	 return 0;
}

int queue_full(void){
	return 0;
}

//To destroy the queue and free the resources
int queue_destroy(void){
	return 0;
}
