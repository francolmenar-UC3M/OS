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
#include <sys/types.h>
 #include <sys/wait.h>

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
	int status,pid, num_rows;            /* Number of processes created by factory_manager, whose parameters are stored in rows */


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

		int param[atoi(aux)][4];  /* Declare the resulting matrix as we know its number of rows */
		size = size -1;
		/* First thing to do is to check if the number of processes parsed are different from 0.
		aux can only be a string of digits from 0 to 9. Therefore, atoi only returns 0 if the actual value is 0, not for any error */
		if (atoi(aux)!=0){
			/* We declare the auxiliary variables that will be used from now */
			num_rows = atoi(aux); /* aux will be used for other purposes, so we store the number of rows of the matrix */

			/* j and k are used for indicating the current position of row (j) and colum (k) */
			int j = 0;
			int k = 0;

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
						/* In the case we are in the first column of the the row, we assign it the number of the column.
						This will be the ID of the process, the only parameter not stated in the input file */
						if (k == 0){
							param[j][k] = j;
							k++;
						}
						/* Now we have the number, we store it in the corresponding position of the resulting matrix param.
						We use atoi again, as it should return 0 only if the number is a 0 -only digits permitted- */
						param[j][k] = atoi(aux);
						/* In the case we have reached the last column (k==3), we update k to 0 and j to j+1 (next row) */
						if (k == 3){
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
/*
		int j;
		int k;
		printf("%i\n",size);
		for(j = 0; j < num_rows; j++){
			for(k = 0; k < 4; k++){
				printf("%i\t",param[j][k] );
			}
			printf("\n" );
		}

		printf("%i\n",num_rows);*/
		char aux_acc[4][20];
		int k;
		printf("joder\n");
		for (i = 0; i<num_rows; i++){
				for(k = 0; k < 4; k++){
					printf("payo %i\n",k);
					sprintf(aux_acc[k],"%i",param[i][k]);
					printf("payo despues %i\n",k);
				}
 		pid = fork();
		switch(pid) {

			case -1:/* Error occurred */
				perror("Error executing fork");
				return -1;

			case 0:/* Child process, the one which executes the process */
			if(execvp("/root/Desktop/2, 2 cuatri/Sistemas/lab3/process_manager.c",aux_acc)<0) { /* Execute the command */
			perror("Error in method execute");                                /* Error message */
			return -1;
			}

			else {
				/* The parent waits until the children has ended */
					while (wait(&status) != pid) {
						/* Check for possible errors */
						if (status != 0) {
							perror("Error waiting the children"); /* Error message */
							return -1;
						}
			}
			printf("Childre arrived\n");
		}
	}
}
	return 0;
}
/* The input file is not a regular file --> invalid file */
else{
	printf("[ERROR][factory_manager] Invalid file\n"); /* Error message */
		exit(-1);
}
}
