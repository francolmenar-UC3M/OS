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
#include <signal.h>
#include <sys/wait.h>


/* Method check_number is used to check if the character entered argv is a number or not.
   In that case, returns 0. Otherwise, returns -1. */
int check_number(char argv){

								/* The number must be in the range of [48,57], this is, [0-9] */
								if((argv>47) && (argv<58)) {
																return 0;
								}
								return -1;
}

/* This class receives a path to a file as input parameter.
   Returns 0 if correct execution, else, returns -1.
 */
int main (int argc, const char * argv[] ){

								struct stat *statbuf; /* Structure to know if the input file is a regular file */
								int fd,n,size;    /* fd -> file descriptor; n -> number of bytes read from the file; size -> size of the file */
								int num_rows;     /* Max number of processes created by factory_manager, whose parameters are stored in rows */
								int status, pid;  /* Variables needed when working with fork() */
								sem_t* sem_factory; /* Name of the semaphore to coordinate all the processes from factory_manager */

								/* argv[0] --> name of the program
								   argv[1] --> name of the input file
								   Therefore, number of arguments must be 2 */
								if(argc!=2) {
																printf("[ERROR][factory_manager] Invalid file\n");
																exit(-1);
								}

								/* Allocate memory to statbuf to not get future errors*/
								statbuf = malloc(sizeof(struct stat));

								/* First, check the stat of the input file */
								if (stat(argv[1],statbuf)<0) {                  /* Get the status to know if it's a regular file */
																printf("[ERROR][factory_manager] Invalid file\n"); /* Error message */
																exit(-1);
								}

								if (S_ISREG(statbuf->st_mode)!=0) {              /* Check if the input file is a regular file */

																if ((fd=open(argv[1],O_RDONLY,0666))<0) { /* Open the input file from argv[1] */

																								printf("[ERROR][factory_manager] Invalid file\n"); /* Error message */
																								exit(-1);
																}

																/* We get the size of the input file to store its full content in the buffer buf */
																if ((size=lseek(fd,0,SEEK_END))<0) {  /* Get the size of the file */

																								printf("[ERROR][factory_manager] Invalid file\n"); /* Error message */
																								exit(-1);
																}

																char buf[size];                       /* buf -> buffer; the size of buffer is the one of the file */

																if ((n=lseek(fd,0,SEEK_SET))<0) {     /* Set the pointer to the beginning of the file again */

																								printf("[ERROR][factory_manager] Invalid file\n"); /* Error message */
																								exit(-1);
																}

																/* Using read, we store the content of the input file in the buffer 'buf' with size 'size', the same of the file */
																if(read(fd,buf,size)<0) {

																								printf("[ERROR][factory_manager] Invalid file\n"); /* Error message */
																								exit(-1);
																}

																/* Close the file */
																if (close(fd)<0) {

																								printf("[ERROR][factory_manager] Invalid file\n"); /* Error message */
																								exit(-1);
																}

																/* PARSER */

																/* Now, we start parsing. The objective of the parser is to store the parameters in a matrix of string in which each row
																   contains the information of each process created. Therefore, each row has three columns. The input file is incorrect when:
																   1. The number of processes that must be created are 0
																   2. The content of the file has at least a character different from a number or a space (this include negative numbers)
																   3. More processes are stated than the ones that must be created
																   4. At least one process has not enough parameters
																   The content of the input file is retrieved from buf */

																/* Now, declare auxiliary variables*/
																int i = 0;        /* i is used for accessing the content of buf via buf[i] */
																char aux[size];   /* aux is used for working with strings. size is the maximum length we work with */

																aux[0] = '\0';    /* At the beginning, aux is empty */
																char aux2[2];     /* aux2 is used for working with strings of size 1 */
																aux2[1] = '\0';   /* Therefore, we know its last position */

																/* Spaces before the first number are allowed. We skip them using a while. 32 is the number for SPACE is ASCII */
																while(buf[i]==32) {
																								i++;
																}
																/* We arrive to the first number, that will be the maximum number of processes created.
																   Check every concatenaded number one by one. Once the char is not a number, skip the while loop.
																   We copy each digit into aux2, to copy them only by one into aux using strcat */
																while(check_number(buf[i])==0) {
																								aux2[0]=buf[i]; /* Copy the digit to aux2 */
																								strcat(aux,aux2); /* Copy each digit to aux, using strcat */
																								i++;
																}

																num_rows = atoi(aux); /* aux will be used for other purposes, so we store the number of rows of the matrix */
																size--;
																char* param[num_rows][6]; /* Declare the resulting matrix as we know its number of rows */

																/* j and k are used for indicating the current position of row (j) and colum (k) */
																int j = 0;
																int k = 0;

																/* semName stores the name of the named semaphore we use for synchronizing processes */
																char* semName = "/sem";

																/* First thing to do is to check if the number of processes parsed are different from 0.
																   aux can only be a string of digits from 0 to 9. Therefore, atoi only returns 0 if the actual value is 0, not for any error */
																if (num_rows!=0) {
																								/* Go over all the content of the buffer, using the counter i -up to size, the length of buf- */
																								while (i<size) {
																																/* Then, we check if the next char in the buffer is an number or not */
																																if (check_number(buf[i])==0) {
																																								/* After knowing the char is a digit, we must ensure that we are not creating more processes than the max.
																																								   This happens when j (actual colum) is equal to the number of rows (max = num_rows -1) */
																																								if (j == num_rows) {
																																																printf("[ERROR][factory_manager] Invalid file\n"); /* Error message */
																																																exit(-1);
																																								}

																																								else{
																																																/* Correct functioning reaching a digit char.
																																																   Reset the value of aux --> empty string */
																																																memset(aux,0,sizeof aux);

																																																/* First we check the preset values:
																																																   Position 0: name of the program we will call "./process"
																																																   Position 0: name of the program we will call "./process"
																																																   Position 2: name of the semaphore, stored in semName
																																																   Position 2: name of the semaphore, stored in semName
																																																   Position 5: NULL, termination of the string. */
																																																if(k==0) {
																																																								param[j][k] = "./process";
																																																								k++;
																																																}

																																																else if(k==2) {/* Concatenation of sem and the id process */
																																																								char str[80];
																																																								strcpy(str, semName);
																																																								strcat(str,param[j][1]);
																																																								param[j][k] = malloc(80*sizeof(char));
																																																								strcpy(param[j][k],str);
																																																								k++;
																																																}

																																																/* In the same case as in the number of processes, we get the full number digit by digit.
																																																   The number is stored in aux digit by digit */
																																																while(check_number(buf[i])==0) {
																																																								aux2[0]=buf[i];
																																																								strcat(aux,aux2);
																																																								i++;
																																																}
																																																/* Now we have the number, we store it in the corresponding position of the resulting matrix param allocating memory
																																																   using malloc to be able to copy the string from aux to param[j][k]. We add a 1 for the NULL termination of the string. */
																																																if((param[j][k]=malloc(sizeof aux + 1))==NULL) {
																																																								printf("[ERROR][factory_manager] Invalid file\n"); /* Error message */
																																																								exit(-1);
																																																}
																																																strcpy(param[j][k],aux); /* Copy the value to param[j][k] from aux */

																																																/* In the case we are in the last element before the NULL, we update k to assign that NULL value
																																																   and go to the following row of the matrix. (Only accessed if more numbers are found) */
																																																if(k==4) {
																																																								k++;
																																																								param[j][k] = NULL;
																																																								k = 0;
																																																								j++;
																																																}
																																																/* For the rest of the cases, just update the row */
																																																else{
																																																								k++;
																																																}
																																								}
																																}
																																/* If the char is not a number, it only can be a SPACE (ASCII 32) to be accepted */
																																else if (buf[i]==32) {
																																								i++; /* Update the counter: next position */
																																}
																																/* If the char is not a number nor a SPACE, the input file is not valid */
																																else{
																																								printf("[ERROR][factory_manager] Invalid file\n"); /* Error message */
																																								exit(-1);
																																}
																								}
																								/* The number of arguments for the processes created are not enough.
																								   This happens when the row has only one parameter in the input file (k=2)
																								   or only two (k=4) --> Invalid file */
																								if(k==2 || k==4) {
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

																sem_t* boss;
																if((boss =sem_open("/boss",O_CREAT,0644,1))==SEM_FAILED) {
																								printf("[ERROR][factory_manager] Process_manager with id %s has finished with errors.\n", param[0][0]); /* Error message */
																								return -1;
																}

																/* Now, we create each of the requested processes in order, passing the following parameters:
																   0 --> ./process (name of the program)
																   1 --> ID of the process (1st parameter of input file)
																   2 --> semName (name of the semaphore)
																   3 --> max element in belt (2nd parameter of input file)
																   4 --> elements to be produced (3rd parameter of input file) */
																sem_t* sem_factory[j]; /* Name of the semaphore to coordinate all the processes from factory_manager */
																int  pids[j];
																for (i = 0; i<j; i++) { /* It goes through each process_manager to be created */

																								/* Open a semaphore for managing the creation of processes. */
																								if((sem_factory[i] = sem_open(param[i][2],O_CREAT,0644,0))==SEM_FAILED) {
																																printf("[ERROR][factory_manager] Process_manager with id %s has finished with errors.\n", param[i][1]); /* Error message */
																																return -1;
																								}

																								pid = fork(); /* Create the child*/
																								switch(pid) {
																								case -1:      /* Error ocurred */
																																printf("[ERROR][factory_manager] Process_manager with id %s has finished with errors.\n", param[i][1]); /* Error message */
																																return -1;

																								case 0:     /* Child process, the one which executes process_manager */

																																if (sem_wait(sem_factory[i])<0) {
																																								printf("[ERROR][factory_manager] Process_manager with id %s has finished with errors.\n", param[i][1]); /* Error message */
																																								return -1;
																																}
																																printf("[OK][factory_manager] Process_manager with id %s has been created.\n",param[i][1]);
																																if(sem_post(boss) < 0) {
																																								printf("[ERROR][factory_manager] Process_manager with id %s has finished with errors.\n", param[i][1]); /* Error message */
																																								return -1;
																																}
																																if(execvp(param[i][0],param[i])<0) { /* Execute process_manager with the parameters of param */
																																								printf("[ERROR][factory_manager] Process_manager with id %s has finished with errors.\n", param[i][1]); /* Error message*/                              /* Error message */
																																								return -1;
																																}
																								/* Free the memory for the parameters stated in the input file, as they are the ones who used malloc */


																								default:
																																pids[i] = pid;
																																sem_wait(boss);
																																sem_post(sem_factory[i]);
																								}
																}
																sleep(1);
																for(i = 0; i < j; i++) {
																								if(sem_post(sem_factory[i]) < 0) {
																																printf("[ERROR][factory_manager] Process_manager with id %s has finished with errors.\n", param[i][1]); /* Error message */
																																return -1;
																								}
																}
																sleep(1);
																for(i = 0; i < j; i++) {
																								if(sem_post(sem_factory[i]) < 0) {
																																printf("[ERROR][factory_manager] Process_manager with id %s has finished with errors.\n", param[i][1]); /* Error message */
																																return -1;
																								}
																								/* The parent waits until the children has ended */
																								waitpid(pids[i], &status, 0);
																																//Check for possible errors


																								printf("[OK][factory_manager] Process_manager with id %s has finished.\n",param[i][1]);
																}
																printf("[OK][factory_manager] Finishing.\n");

/* Close the semaphore we have used to synchronize the processes. If the result is negative --> error */
																if(sem_close(sem_factory[i])<0) {
																								printf("[ERROR][factory_manager] Process_manager with id %s has finished with errors.\n", param[i][1]); /* Error message*/
																								return -1;

																}
/* Remove the named semaphore referred by semName (free resources). If the result is negative --> error */
																if(sem_unlink(param[i][2])<0) {
																								printf("[ERROR][factory_manager] Process_manager with id %s has finished with errors.\n", param[i][1]); /* Error message*/
																								return -1;
																}
								}
/* The input file is not a regular file --> invalid file */
								else{
																printf("[ERROR][factory_manager] Invalid file\n"); /* Error message */
																exit(-1);
								}
								printf("[OK][factory_manager] Finishing.\n");
								return 0;
}
