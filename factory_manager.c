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

int parser (char *buf, int size){
	
	int i = 0;
	char aux[size];
	
	while(check_number(buf[i])==0){
		str_append(aux,buf[i]);
		i++;
	}
	
	int param[atoi(aux)][3];
	
	i = 0;
	int j = 0;
	int k = 0;
	
	while (buf[i]!=EOF){
		if (check_number(buf[i])==0){
			memset(aux,0,sizeof aux);
			while(check_number(buf[i])==0){
				str_append(aux,buf[i]);
				i++;
			}
			param[j][k] = atoi(aux);
			if (k = 2){
				k = 0;
				j++;
			}
			else{
				k++;
			}
		}
		else if (buf[i]==' '){
			i++;
		}
		else{
			perror("[ERROR][factory_manager] Invalid file\n"); /* Error message */
			return -1;
		}
	}
	return 0;
   }

int check_number(char *argv){
	int i = 0;                     /* Aux variable */
	int len = strlen(argv);        /* len stores the lenght of the string stored in argvv, this is, the number of digits */
	int num_processes;
	
	
	/* The loop returns 0 if structure -/+<number>; else, returns -1 */
	while(i<len) {
		if((argv[i]>47) && (argv[i]<58)) {                      /* The char is a number */
		i++;                                                    /* Next position */
		}
		else{                                                   /* The char is other type*/
		return -1;                                              /* Skip the loop */
		}
		}
	return 0;
}

/* This class receives a path to a file as input parameter.
Returns 0 if correct execution, else, returns -1.
*/
int main (int argc, const char * argv[] ){
	
	struct stat *statbuf;    /* Structure to know if the input file is a regular file */
	int fd,n,size;           /* fd -> file descriptor; n -> number of bytes read from the file; size -> size of the file */
	
    char *buf;               /* buf -> buffer; the size of buffer is the one of the file */

	/* argv[0] --> name of the program
	   argv[1] --> name of the input file 
	   Therefore, number of arguments must be 2 */
	if(argc!=2){
		printf("[ERROR][factory_manager] Invalid file\n");
		exit(-1);
	}
	
	statbuf = malloc(sizeof(struct stat));                 /* Allocate memory to statbuf */
	
	/* Check the stat of the input file */
	if (stat(argv[1],statbuf)<0){                          /* Get the status to know if it's a regular file */
		perror("[ERROR][factory_manager] Invalid file\n"); /* Error message */
		exit(-1);
	}
	
	if (S_ISREG(statbuf->st_mode)!=0) {                     /* Check if the input file is a regular file */
		
		if ((fd=open(argv[1],O_RDONLY,0666))<0) {           /* Open the input file from argv[1] */
		
    	perror("[ERROR][factory_manager] Invalid file\n");  /* Error message */
		exit(-1);
		}
		
		if ((size=fseek(fd,0,SEEK_END))<0){                 /* Get the size of the file */
			
		perror("[ERROR][factory_manager] Invalid file\n");  /* Error message */
		exit(-1);
		}
		
		if ((n=read(fd,buf,size))<0){                /* Read the input file given the size of the file */
			
		perror("[ERROR][factory_manager] Invalid file\n");  /* Error message */
		exit(-1);
		}
		
		/* Check for errors when reading input file */
		if (n<0){
		
		perror("[ERROR][factory_manager] Invalid file\n");  /* Error message */
		exit(-1);
		}
		
		/* Close the file */
		if (close(fd)<0){
		
		perror("[ERROR][factory_manager] Invalid file\n"); /* Error message */
		exit(-1);
		}
		
		int buf[size];
		
		parser(buf, size);
		
	}
	else {
		printf("[ERROR][factory_manager] Invalid file\n");
		exit(-1);
	}

	return 0;
}
