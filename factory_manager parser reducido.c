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
#include <string.h>


#define BUFFER_SIZE 1024
#define STDOUT_FILENO 1


/*
   It checks if a string is a number or not
   If it is a number 0 is returned
   Otherwise a -1 is returned
 */
int check_number(char *argv){
								int i = 0;              /* Aux variable */
								int len = strlen(argv); /* len stores the lenght of the string stored in argvv, this is, the number of digits */
								int num_processes;
								/* The loop returns 0 if structure -/+<number>; else, returns -1 */
								while(i<len) {
																if((argv[i]>47) && (argv[i]<58)) {        /* The char is a number */
																								i++;                              /* Next position */
																}
																else{                                     /* The char is other type*/
																								return -1;                        /* Skip the loop */
																}
								}
								return 0;
}

int parser (char *buf, int size){
								char * pch;
								int *arr;
								int sizeArr = 0;//The size of the in array
								pch = strtok (buf," ");
								printf("Primer while\n");
								while (pch != NULL) //I calculate the size of the array
								{
																//printf ("%s\n",pch);
																pch = strtok (NULL, " ");
																sizeArr++;
								}
								//arr[sizeArr];
								printf("%i\n", sizeArr);
								arr = malloc(sizeof(int)*(sizeArr-1));
								pch = strtok (buf," ");
								sizeArr=0;
								printf("Segundo while\n");
								while (pch != NULL) //I calculate the size of the array
								{
																//printf ("%s\n",pch);
																pch = strtok (NULL, " ");
																printf("dentro\n");
																if(check_number(pch)==0){
																	printf("La liamos\n");
																	arr[sizeArr] = atoi(pch);
																}
																else return -1;
																sizeArr++;
								}

								return 0;
}



/* This class receives a path to a file as input parameter.
   Returns 0 if correct execution, else, returns -1.
 */
int main (int argc, const char * argv[] ){

								struct stat *statbuf; /* Structure to know if the input file is a regular file */
								int fd,n,size;    /* fd -> file descriptor; n -> number of bytes read from the file; size -> size of the file */

								char buf[BUFFER_SIZE];           /* buf -> buffer; the size of buffer is the one of the file */

								/* argv[0] --> name of the program
								   argv[1] --> name of the input file
								   Therefore, number of arguments must be 2 */
								if(argc!=2) {
																printf("[ERROR][factory_manager] Invalid file argc\n");
																exit(-1);
								}

								statbuf = malloc(sizeof(struct stat));          /* Allocate memory to statbuf */

								/* Check the stat of the input file */
								if (stat(argv[1],statbuf)<0) {                  /* Get the status to know if it's a regular file */
																perror("[ERROR][factory_manager] Invalid file stat\n"); /* Error message */
																exit(-1);
								}

								if (S_ISREG(statbuf->st_mode)!=0) {            /* Check if the input file is a regular file */

																if ((fd=open(argv[1],O_RDONLY,0666))<0) { /* Open the input file from argv[1] */

																								perror("[ERROR][factory_manager] Invalid file\n"); /* Error message */
																								exit(-1);
																}

																if ((size=lseek(fd,0,SEEK_END))<0) {   //Get the size of the file

																								perror("[ERROR][factory_manager] Invalid file\n");  //Error message
																								exit(-1);
																}
																buf[size] = 0;
																lseek(fd,0,SEEK_SET);

																if ((n=read(fd,buf,size))<0) { //Read the input file given the size of the file

																								perror("[ERROR][factory_manager] Invalid file read\n"); // Error message
																								exit(-1);
																}
																printf("Entramos\n");
																parser(buf,size);
																printf("Salimos\n");

																/* Check for errors when reading input file */
																if (n<0) {

																								perror("[ERROR][factory_manager] Invalid file n \n"); /* Error message */
																								exit(-1);
																}

																/* Close the file */
																if (close(fd)<0) {

																								perror("[ERROR][factory_manager] Invalid file close\n"); /* Error message */
																								exit(-1);
																}

																//int buf[size];
																//parser(buf, size);

								}
								else {
																printf("[ERROR][factory_manager] Invalid file else\n");
																exit(-1);
								}

								return 0;

}
