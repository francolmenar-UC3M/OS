/*
 *
 * factory_manager.c
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stddef.h>
#include <semaphore.h>
#include <sys/stat.h>

/* This class receives a path to a file as input parameter.
Returns 0 if correct execution, else, returns -1.
*/
int main (int argc, const char * argv[] ){

	/* argv[0] --> name of the program
	   argv[1] --> name of the input file 
	   Therefore, number of arguments must be 2 */
	if(argc!=2){
		printf("[ERROR][factory_manager] Invalid file");
	}

	return 0;
}
