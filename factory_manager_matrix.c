*
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

int check_number(char argv){
	
	if((argv>47) && (argv<58)){
		return 0;
	}
	return -1;
}

int parser (char* buf, int size){
	
	int i = 0;
	char aux[size];
	char aux2[2];
	printf("0: %c\n",buf[0]);
	printf("1: %c\n",buf[1]);
	printf("2: %c\n",buf[2]);
	printf("3: %c\n",buf[3]);
	
	while(check_number(buf[i])==0){
		printf("i: %c\n",i,buf[i]);
		aux2[0]=buf[i];
		strcat(aux,aux2);
		i++;
	}
	
	
	int param[atoi(aux)][3];
	
	int j = 0;
	int k = 0;
	
	while (buf[i]!=EOF){
		if (check_number(buf[i])==0){
			printf("%i: %c\n",i,buf[i]);
			memset(aux,0,sizeof aux);
			while(check_number(buf[i])==0){
				aux2[0]=buf[i];
				strcat(aux,aux2);
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
		else if (buf[i]==32){
			printf("%i: %c\n",i,buf[i]);
			i++;
		}
		else{
			printf("%i: %c\n",i,buf[i]);
			printf("[ERROR][factory_manager] Invalid file 1\n"); /* Error message */
			return -1;
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
		printf("[ERROR][factory_manager] Invalid file\n"); /* Error message */
		exit(-1);
	}
	
	if (S_ISREG(statbuf->st_mode)!=0) {                     /* Check if the input file is a regular file */
		
		if ((fd=open(argv[1],O_RDONLY,0666))<0) {           /* Open the input file from argv[1] */
		
    	printf("[ERROR][factory_manager] Invalid file\n");  /* Error message */
		exit(-1);
		}
		
		if ((size=lseek(fd,0,SEEK_END))<0){                 /* Get the size of the file */
			
		printf("[ERROR][factory_manager] Invalid file\n");  /* Error message */
		exit(-1);
		}
		
		buf = malloc(size);
		
		if((n=read(fd,buf,size))<0) {
			
			printf("[ERROR][factory_manager] Invalid file\n"); /* Error message */
			exit(-1);
		}
		
		/* Close the file */
		if (close(fd)<0){
		
		printf("[ERROR][factory_manager] Invalid file\n"); /* Error message */
		exit(-1);
		}
		
		parser(buf, size);
		
		free(buf);
		
	}
	else {
		printf("[ERROR][factory_manager] Invalid file\n");
		exit(-1);
	}

	return 0;
}
