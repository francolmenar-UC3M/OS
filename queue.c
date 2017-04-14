#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include "queue.h" /* https://github.com/stockrt/queue.h/blob/master/sample.c */

struct object
{
	int num_edition;  // represents the order of creation inside the belt. It will start from 0 and will be incremented by one for each new element.
	int id_belt; // represents the id of the belt in which the object is created (this identifier is passed as a parameter to the program).
	int last; // it will be 0 if it is not the last element to be inserted in the belt and 1 if it is the last one to be inserted.
};


struct queue
{
    int capacity;  // maximum number of items in the queue
    int count;     // number of items in the queue
    struct object *head;       // pointer to head
    struct object *tail;       // pointer to tail
};


/*
	Function that creates the queue and reserves
	memory for the size specified as parameter.
*/
int queue_init(int size){
	struct queue *circular_buffer;
	circular_buffer = malloc(sizeof(struct queue));
	circular_buffer -> capacity = size;	
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
int main (int argc, const char * argv[] ){
	
	return 0;
}
