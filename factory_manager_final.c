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

/* Method check_number is used to check if the character entered argv is a number or not.
In that case, returns 0. Otherwise, returns -1. */
int check_number(char argv){
	
	/* The number must be in the range of [48,57], this is, [0-9] */
	if((argv>47) && (argv<58)){
		return 0;
	}
	return -1;
}

/* This class receives a path to a file as input parameter.
Returns 0 if correct execution, else, returns -1.
*/
int main (int argc, const char * argv[] ){
	
	struct stat *statbuf;    /* Structure to know if the input file is a regular file */
	int fd,n,size;           /* fd -> file descriptor; n -> number of bytes read from the file; size -> size of the file */
	int num_rows;            /* Number of processes created by factory_manager, whose parameters are stored in rows */
	int status, pid;         /* Variables needed when working with fork() */
	sem_t* sem_factory;              /* Name of the semaphore to coordinate all the processes from factory_manager */

	/* argv[0] --> name of the program
	   argv[1] --> name of the input file 
	   Therefore, number of arguments must be 2 */
	if(argc!=2){
		printf("[ERROR][factory_manager] Invalid file\n");
		exit(-1);
	}
	
	/* Allocate memory to statbuf to not get future errors*/
	statbuf = malloc(sizeof(struct stat));                 
	
	/* First, check the stat of the input file */
	if (stat(argv[1],statbuf)<0){                          /* Get the status to know if it's a regular file */
		printf("[ERROR][factory_manager] Invalid file\n"); /* Error message */
		exit(-1);
	}
	
	if (S_ISREG(statbuf->st_mode)!=0) {                     /* Check if the input file is a regular file */
		
		if ((fd=open(argv[1],O_RDONLY,0666))<0) {           /* Open the input file from argv[1] */
		
    	printf("[ERROR][factory_manager] Invalid file\n");  /* Error message */
		exit(-1);
		}
		
		/* We get the size of the input file to store its full content in the buffer buf */
		if ((size=lseek(fd,0,SEEK_END))<0){                 /* Get the size of the file */
			
		printf("[ERROR][factory_manager] Invalid file\n");  /* Error message */
		exit(-1);
		}
		
		
		char buf[size];                                     /* buf -> buffer; the size of buffer is the one of the file */
		
		if ((n=lseek(fd,0,SEEK_SET))<0){                    /* Set the pointer to the beginning of the file again */
			
		printf("[ERROR][factory_manager] Invalid file\n");  /* Error message */
		exit(-1);
		}
		
		/* Using read, we store the content of the input file in the buffer 'buf' with size 'size', the same of the file */
		if(read(fd,buf,size)<0) {
			
			printf("[ERROR][factory_manager] Invalid file\n"); /* Error message */
			exit(-1);
		}
 
		/* Close the file */
		if (close(fd)<0){
		
		printf("[ERROR][factory_manager] Invalid file\n"); /* Error message */
		exit(-1);
		}
		
		/* PARSER */
		/* Now, we start parsing. The objective of the parser is to store the parameters in a matrix in which each row 
		contains the information of each process created. Therefore, each row has three columns. The input file is incorrect when:
		1. The number of processes that must be created are 0
		2. The content of the file has at least a character different from a number or a space (this include negative numbers)
		3. More processes are stated than the ones that must be created
		4. At least one process has not enough parameters 
		The content of the input file is retrieved from buf */
		
		/* Now, declare auxiliary variables*/
		int i = 0;                      /* i is used for accessing the content of buf via buf[i] */
		char aux[size];                 /* aux is used for working with strings. size is the maximum length we work with */
	
		aux[0] = '\0';                  /* At the beginning, aux is empty */
		char aux2[2];                   /* aux2 is used for working with strings of size 1 */
		aux2[1] = '\0';                 /* Therefore, we know its last position */      
		
		/* Spaces before the first number are allowed. We skip them using a while. 32 is the number for SPACE is ASCII */
		while(buf[i]==32){
			i++;
		}
		/* We arrive to the first number, that will be the maximum number of processes created.
		Check every concatenaded number one by one. Once the char is not a number, skip the while loop. 
		We copy each digit into aux2, to copy them only by one into aux using strcat */
		while(check_number(buf[i])==0){
			aux2[0]=buf[i];             /* Copy the digit to aux2 */
			strcat(aux,aux2);           /* Copy each digit to aux, using strcat */
			i++;
		}
		
		int param[atoi(aux)][3];  /* Declare the resulting matrix as we know its number of rows */
		
		/* j and k are used for indicating the current position of row (j) and colum (k) */
			int j = 0;
			int k = 0;
		
		/* First thing to do is to check if the number of processes parsed are different from 0.
		aux can only be a string of digits from 0 to 9. Therefore, atoi only returns 0 if the actual value is 0, not for any error */
		if (atoi(aux)!=0){
			/* We declare the auxiliary variables that will be used from now */
			num_rows = atoi(aux); /* aux will be used for other purposes, so we store the number of rows of the matrix */
			
			/* Go over all the content of the buffer, using the counter i -up to size, the length of buf- */
			while (i<size){
				/* First we check if the current char is a number or not */
				if (check_number(buf[i])==0){
					/* After knowing the char is a digit, we must ensure that we are not creating more processes than the max.
					This happens when j (actual colum) is equal to the number of rows (max = num_rows -1) */
					if (j == num_rows){
					printf("[ERROR][factory_manager] Invalid file\n"); /* Error message */
					exit(-1);
					}
					else{
					/* Correct functioning reaching a digit char.
					Reset the value of aux --> empty string */
					memset(aux,0,sizeof aux);
					
					/* In the same case as in the number of processes, we get the full number digit by digit.
					The number is stored in aux digit by digit */
					while(check_number(buf[i])==0){
						aux2[0]=buf[i];
						strcat(aux,aux2);
						i++;
					}
						/* Now we have the number, we store it in the corresponding position of the resulting matrix param.
						We use atoi again, as it should return 0 only if the number is a 0 -only digits permitted- */
						param[j][k] = atoi(aux);
						/* In the case we have reached the last column (k==3), we update k to 0 and j to j+1 (next row) */
						if (k == 2){
							k = 0;
							j++;
						}
						/* If we are not in the last colum, we simply update the column k to k+1 */
						else{
							k++;
						}	
					}
				}
				/* If the char is not a number, it only can be a SPACE (ASCII 32) to be accepted */
				else if (buf[i]==32){
					i++; /* Update the counter: next position */
				}
				/* If the char is not a number nor a SPACE, the input file is not valid */
				else{
					printf("[ERROR][factory_manager] Invalid file\n"); /* Error message */
					exit(-1);
				}
			}
			/* The number of arguments for the processes created are not enough.
			This happens when the last parameter is not set at any column with k = 2.
			As we update always the k, after parsing the file, the value for k should be 0.
			Otherwise, invalid file */
			if(k!=0){
				printf("[ERROR][factory_manager] Invalid file\n"); /* Error message */
				exit(-1);
			}
		}
		/* The number of processes is equal to 0 --> invalid file */
		else{
			printf("[ERROR][factory_manager] Invalid file\n"); /* Error message */
			exit(-1);		
		}
		/* PARSER ENDS */
		/* SYNCHRONIZATION STARTS */
		
		char *aux_acc[6];         /* It will be the arguments passed to the process_manager */ 
		aux_acc[0] = "./process"; /* It is the name of the executable of the process */
		aux_acc[5] = NULL;        /* The end of the string variable */
		
		char* semName = "/sem";
		/* Open a semaphore for managing the creation of processes. */
		
		if((sem_factory =sem_open(semName,O_CREAT,0644,0))==SEM_FAILED){
			printf("[ERROR][factory_manager] Process_manager with id %i has finished with errors.\n", param[0][0]); // Error message
  			return -1;
		}
		if(sem_close(sem_factory)<0){
 				printf("[ERROR][factory_manager] Process_manager with id %i has finished with errors sem.\n", param[i][0]); /* Error message*/                        
                return -1;
  		}		
		for (i = 0; i<j; i++){       /* It goes for each process_manager to be created */
			for(k = 0; k < 4; k++){  /* It goes through all the variables of a given process_manager */
			
				if(k==0){
					int length;
				if ((length=snprintf(NULL,0,"%d",param[i][k]))<0){ /* We calculate the length of the integer */
					printf("[ERROR][factory_manager] Process_manager with id %i has finished with errors.\n", param[i][0]);
					return -1;
				}
				/* We allocate the memory for the auxiliar buffer and for the buffer which will be passed to the process_manager*/
				char* auxiliar;
				if((auxiliar=malloc (length +1))==NULL){ 
					printf("[ERROR][factory_manager] Process_manager with id %i has finished with errors.\n", param[i][0]);
					return -1;
				}
				if((aux_acc[k+1] = malloc (length +1))==NULL){
					printf("[ERROR][factory_manager] Process_manager with id %i has finished with errors.\n", param[i][0]);
					return -1;
				}
				if((snprintf(auxiliar,length + 1,"%d",param[i][k]))<0){ /* Convert the int into a string */
					printf("[ERROR][factory_manager] Process_manager with id %i has finished with errors.\n", param[i][0]);
					return -1;
				}
					strcpy(aux_acc[k+1],auxiliar); /* Copy the string from the auxiliary buffer to the correct one */
					free(auxiliar);                /* Free the memory corresponding to the auxiliary buffer */
				}			
				else if(k == 1){
 					int length;
 					length=strlen(semName);/* We calculate the length of the integer */
 					if((aux_acc[2] = malloc (length +1))==NULL){
 					printf("[ERROR][factory_manager] Process_manager with id %i has finished with errors mal2.\n", param[i][0]);
  					return -1;
 					}
					strcpy(aux_acc[2],semName); /*  semaphore's name */
				}
				else{
					int length;
				if ((length=snprintf(NULL,0,"%d",param[i][k]))<0){ /* We calculate the length of the integer */
					printf("[ERROR][factory_manager] Process_manager with id %i has finished with errors.\n", param[i][0]);
					return -1;
				}
				/* We allocate the memory for the auxiliar buffer and for the buffer which will be passed to the process_manager*/
				char* auxiliar;
				if((auxiliar=malloc (length +1))==NULL){ 
					printf("[ERROR][factory_manager] Process_manager with id %i has finished with errors.\n", param[i][0]);
					return -1;
				}
				if((aux_acc[k+1] = malloc (length +1))==NULL){
					printf("[ERROR][factory_manager] Process_manager with id %i has finished with errors.\n", param[i][0]);
					return -1;
				}
				if((snprintf(auxiliar,length + 1,"%d",param[i][(k-1)]))<0){ /* Convert the int into a string */
					printf("[ERROR][factory_manager] Process_manager with id %i has finished with errors.\n", param[i][0]);
					return -1;
				}
					strcpy(aux_acc[k+1],auxiliar); /* Copy the string from the auxiliary buffer to the correct one */
					free(auxiliar);                /* Free the memory corresponding to the auxiliary buffer */
				}			
			}
			
			pid = fork(); /* Create the child*/
			switch(pid){
				
				case -1:  /* Error ocurred */
				printf("[ERROR][factory_manager] Process_manager with id %i has finished with errors.\n", param[i][0]);
				return -1;
				
				case 0: /* Child process, the one which executes process_manager */
				printf("[OK][factory_manager] Process_manager with id %i has been created.\n",param[i][0]);
				
				if(execvp(aux_acc[0],aux_acc)<0) {  /* Execute process_manager */
					printf("[ERROR][factory_manager] Process_manager with id %i has finished with errors.\n", param[i][0]); /* Error message*/                        				  /* Error message */
					return -1;
				}
				//sem_wait(sem_factory);
				
				default: /* The parent waits until the children has ended */
					while (wait(&status) != pid) {
						/* Check for possible errors */
						if (status != 0) {
							printf("[ERROR][factory_manager] Process_manager with id %i has finished with errors.\n", param[i][0]); /* Error message */
							return -1;
						}
					}
					printf("[OK][factory_manager] Process_manager with id %i has finished.\n",param[i][0]);
			}
		}
			printf("[OK][factory_manager] Finishing.\n");			
  			if(sem_unlink(semName)<0){
 				printf("[ERROR][factory_manager] Process_manager with id %i has finished with errors sem1.\n", param[i][0]); /* Error message*/   
				return -1;
  			}
			return 0;			
	}
	/* The input file is not a regular file --> invalid file */
	else{
		printf("[ERROR][factory_manager] Invalid file\n"); /* Error message */
			exit(-1);
	}
}
