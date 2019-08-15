/*
* This program implements a shell which take command from user for each line. The sequence of arguments separated by either an
* arbitrary number of spaces or one of the two I/O redirection characters (“<” and “>”).
*/

// libraries
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>
#include <assert.h>

// static values
#define MAX_NUM_ARG 30
#define BUFFER_SIZE 256 
// NEW: Nothing happens automatically in C, and there are no “default values”.
// NEW: No input line will exceed 1024 bytes. So Buffer size should be 1024/4 = 256
// NEW: largest data type in c is 4 bytes


// program structure
struct Program{
	char name[BUFFER_SIZE];
	char* arguments[MAX_NUM_ARG];
	int numArguments;
};

// forward declaration of the functions
void setProgramAndArguments(char *buffer, struct Program *program);
void convertToAbsolutePath(char path[]);
int convertToAbsoluteProgram(char programName[]);
char * getFileName(char *buffer, int index);
char * substr(char * s, int x, int y);
void trimLeading(char *str);
int getIndexOfArray(int val, int *arr, int size);


// starting of the application
int	main(int argc, char * argv[]) {

	// flag variables
	int commandFlag = 1;
	int isBackgroundProcess;
	int isOnlyOutRedirect, isOnlyInRedirect, isBothInOutRedirect;

	char *ioInFileName, *ioOutFileName;

	// user input - command
	char command[BUFFER_SIZE];
	char commandUptoRedirection[BUFFER_SIZE], tmpCommand[BUFFER_SIZE];
	
	struct Program program;

	int i, j; // loop control variable
	int fd, in, out; // file descriptor variable
	int returnValue; // return value from executing the program
	int status; // status of the process
	int arrayIndex; // index of the element in an array
	int changeDirStatus; // status of changing directory process

	// variables for background processes
	int processIdArr[BUFFER_SIZE], processIdArrSize = 0; // process id list & size
	char commandList[BUFFER_SIZE][BUFFER_SIZE]; // command list


	// read command & executing program until user enters exit
	while(commandFlag == 1) {

		// initialize the flag variables
		isOnlyInRedirect = 0, isOnlyOutRedirect = 0, isBothInOutRedirect = 0;
		isBackgroundProcess = 0;

		// print "MyShell:" after every command
		printf("MyShell: ");
		fgets(command, BUFFER_SIZE, stdin);
		// NEW: fgets to read string from standard input (Console Read String)
		// NEW: stdin(standard input) is the file pointer, device is keyboard. 
		// NEW: file pointer is for accessing the file for read/write.
		// NEW: if the user input goes beyond BUFFER_SIZE, it discard the rest of the input beyond that.
		// NEW: https://www.tutorialspoint.com/cprogramming/c_input_output.htm

		command[strlen(command) - 1] = '\0'; // NEW: replace '\n'(or last char if input size beyond BUFFER_SIZE) by '\0'
		trimLeading(command); // remove leading spaces/tabs

		// copy command to other temp command variables
		memcpy(commandUptoRedirection, command, sizeof commandUptoRedirection); 
		memcpy(tmpCommand, command, sizeof tmpCommand);

		if (strcmp(command, "exit") == 0) { // check & change flag variable if user enters exit
			commandFlag = 0;

		} else if (command[0] == '\0') { // NEW: if user enter only enter or enter with spaces; it will replace '\n' by '\0'
			// do nothing, just continue

		} else {
			
			// check input/output redirection
			for (i=0; i<strlen(command); i++) {
                if (command[i] == '>' || command[i] == '<') {
                	if (command[i] == '>') {						
	                	isOnlyOutRedirect = 1;
	                	ioOutFileName = getFileName(command, i);
	                	for (j=i+1; j<strlen(command); j++) {
							if (command[j] == '<') {
								isBothInOutRedirect = 1;
								isOnlyOutRedirect = 0;
								ioInFileName = getFileName(command, j);
								break; // if another redirection found, no need to continue the loop
							}
						}

						break; // after the 2nd loop execution no need to continue the 1st loop
	                	
                	} else {
						isOnlyInRedirect = 1;
						ioInFileName = getFileName(command, i);
						for (j=i+1; j<strlen(command); j++) {
							if (command[j] == '>') {
								isBothInOutRedirect = 1;
								isOnlyInRedirect = 0;
								ioOutFileName = getFileName(command, j);
								break; // if another redirection found, no need to continue the loop
							}
						}

						break; // after the 2nd loop execution no need to continue the 1st loop
                	}
                }
            }

        	// get the substring of the command upto the 1st redirectionOperator appears
        	// NEW: copy string from 0 to i-1      	
        	memcpy(commandUptoRedirection, &command[0], i-1);
        	commandUptoRedirection[i] = '\0';
         	// printf("commandUptoRedirection: %s\n", commandUptoRedirection);

            // check banckground process
			if (strcmp(strtok(tmpCommand, " "), "bg") == 0) {
				isBackgroundProcess = 1;
				// get the substring after bg to end of the command
				memcpy(commandUptoRedirection, &command[2], strlen(command)-1);
				commandUptoRedirection[strlen(commandUptoRedirection)] = '\0';
			}

			// set & get the program name & arguments
			// printf("commandUptoRedirection: %s\n", commandUptoRedirection);
			setProgramAndArguments(commandUptoRedirection, &program);

			// printf("Program: %s\n", program.name);
			// printf("Program Argument 0: %s\n", program.arguments[0]);
			// printf("Program Argument 1: %s\n", program.arguments[1]);
			// printf("Program Argument 2: %s\n", program.arguments[2]);

			// execute the program
			if (strcmp(program.name, "cd") == 0) { // program - cd			
				if (program.numArguments >= 3) {
					// convert the path to absolute path, if it is relative path based on the cwd
					convertToAbsolutePath(program.arguments[1]);
					
					changeDirStatus = chdir(program.arguments[1]); // change directory, if valid path
					if (changeDirStatus != 0) { // check if successful changing directory
						printf("Error: Not valid directory. Please enter valid directory name or path!\n"); 
					}
				}

			} else if (strcmp(program.name, "processes") == 0) { // program - processes
				printf("PID     CMD\n");
				printf("---     ---\n");
				for(i=0; i<processIdArrSize; i++) {
					// NEW: The pid_t data type represents process IDs. You can get the process ID of a process by calling getpid. The function getppid returns the process ID of the parent of the current process.
					// NEW: WNOHANG causes the call to waitpid to return status information immediately, without waiting for the specified process to terminate. Normally, a call to waitpid causes the calling process to be blocked until status information from the specified process is available; the WNOHANG option prevents the calling process from being blocked. If status information is not available, waitpid returns a 0. 
					// NEW: WUNTRACED causes the call to waitpid to return status information for a specified process that has either stopped or terminated. Normally, status information is returned only for terminated processes.
					// NEW: If WNOHANG was specified and one or more child(ren) specified by pid exist, but have not yet changed state, then 0 is returned.
					pid_t processStatus = waitpid(processIdArr[i], &status, WNOHANG); 
					if (processStatus == 0) {
						// kill(processIdArr[i], SIGTERM);
						// printf("#Process Id: %d   Command: %s \n", processIdArr[i], commandList[i]);
						printf("%d    %s \n", processIdArr[i], commandList[i]);						
					}			        
			    }

			} else { // handle all other processes with/without i/o redirection
				pid_t processId = fork(); //create child process

				if (processId == 0) { // inside child process

					// NEW: dup/dup2 - Technically the purpose is to share one File table Entry inside a single process by different handles. 
					// (If we are forking the descriptor is duplicated by default in the child process and the file table entry is also shared).
					// That means we can have more than one file descriptor having possibly different attributes for one single open file table entry using dup/dup2 function.
					if (isBothInOutRedirect) { // both input output redirection
						// NEW: open() return the new file descriptor, or -1 if an error occurred
						in = open(ioInFileName, O_RDONLY);
						dup2(in, STDIN_FILENO);
						close(in);
						out = open(ioOutFileName, O_WRONLY | O_CREAT, 0666);
						dup2(out, STDOUT_FILENO);
						close(out);

					} else if (isOnlyInRedirect) { // input redirection
						fd = open(ioInFileName, O_RDONLY);
                    	close(0);
                    	dup(fd);
                    	// close(fd);

					} else if (isOnlyOutRedirect) { //output redirection
						// NEW: O_CREAT --> If pathname does not exist, create it as a regular file.
						// NEW: O_TRUNC --> If the file already exists and is a regular file and the access mode allows writing (i.e., is O_RDWR or O_WRONLY) it will be truncated to length 0.
						fd = open(ioOutFileName, O_WRONLY | O_TRUNC | O_CREAT, 0644);
                        close(1);
                        // NEW: The dup system call duplicates an existing file descriptor, returning a new one that refers to the same underlying I/O object.
                        dup(fd);
                        // close(fd);
					}

					// execute the program or process
					if (convertToAbsoluteProgram(program.name) == 1) {
						if (isBothInOutRedirect) {
							returnValue = execlp(program.name, program.name, NULL);	
						} else {
							returnValue = execv(program.name, program.arguments);	
						}
									
						//check if program can be executed successfully
						if (returnValue == -1) {
							fprintf(stderr, "Error: Program %s not executed!\n", program.name);
						}							

					} else { // could not found the program
						fprintf(stderr, "Error: Program %s not found!\n", program.name);	
					}

					if (isOnlyInRedirect || isOnlyOutRedirect) {
						close(fd);		
					}

					exit(0); // exits child process

				} else if (processId < 0) {
					printf("Error: Forking failed!\n");

				} else { // inside parent process
					if (!isBackgroundProcess) {
						// NEW: The waitpid() system call suspends execution of the calling process until a child specified by pid argument has changed state.
						waitpid(processId, 0, 0);

					} else { 
						int arrayIndex = getIndexOfArray(processId, processIdArr, processIdArrSize); // check if the new process id exists or not
						if (arrayIndex == -1) { // process id not exists into the processIdList
							processIdArr[processIdArrSize] = processId;
							strcpy(commandList[processIdArrSize], command);	
							processIdArrSize++;

						} else {
							strcpy(commandList[arrayIndex], command);
						}						
					}
				}			

				// free arguments of the program
				for (i = 0; i < program.numArguments - 1; i++) {
					free (program.arguments[i]);
				}				
			}						
		}
	}

	return 0;
}

/*
 * find the index of the element inside of an array; if not exists it returns -1.
 */
int getIndexOfArray(int val, int *arr, int size) {
    for (int i=0; i < size; i++) {
        if (arr[i] == val)
            return i;
    }
    return -1;
}

/*
 * find the filename if i/o redirection
 * output: file name string
 */
char * getFileName(char *buffer, int index) {
    int i, j = 0; //size of the filename
    char *fileName = (char *)malloc(BUFFER_SIZE*sizeof(char));
    char *newBuffer = substr(buffer, index+1, strlen(buffer));
    trimLeading(newBuffer);

    for (i=0; i<strlen(newBuffer); i++) {
    	if ((newBuffer[i] == ' ') || (newBuffer[i] == '<') || (newBuffer[i] == '>') || (newBuffer[i] == '\0')) {
        	break;
    	} else {
            fileName[j++] = newBuffer[i];
    	}
    }

    fileName[j++] = '\0';
    return fileName;
}

/*
* get the substring with start and end index
*/
char * substr(char * s, int x, int y) {
    char * ret = malloc(strlen(s) + 1);
    char * p = ret;
    char * q = &s[x];

    assert(ret != NULL);

    while(x  < y) {
        *p++ = *q++;
        x ++; 
    }

    *p++ = '\0';

    return ret;
}

/*
 * remove leading whitespace characters from string
 */
void trimLeading(char *str){
    int index = 0, i, j;

    // find last index of whitespace character
    while(str[index] == ' ' || str[index] == '\t' || str[index] == '\n') {
        index++;
    }

    if(index != 0) {
        i = 0;
        while(str[i + index] != '\0') {
            str[i] = str[i + index];
            i++;
        }
        str[i] = '\0';
    }
}

/*
 * search program name in PATH environment; if not found in PATH environment, it search on current working directory
 * return 1: if program found, program name is changed to absolute program name path
 * return -1: if program not found
 */
int convertToAbsoluteProgram(char programName[]) {
	if (access(programName, X_OK) == 0) { //NEW: access() checks whether the calling process can access the file pathname. X_OK test whether the file exists and grants read, write, and execute permissions
		return 1;
	}	
	
	char * currentPath;
	// NEW: read PATH enviroment. initial PATH variable is usually set in /etc/profile
	// NEW: PATH is an environment variable on Unix-like operating systems, DOS, OS/2, and Microsoft Windows, specifying a set of directories where executable programs are located.
	char * pathPointer = getenv("PATH");
	char currentPathBuffer[BUFFER_SIZE]; // current path
	char tempProgram[BUFFER_SIZE]; // program buffer

	// check in PATH environment
	char* pathToken = strtok(pathPointer, ":"); // parse path by :
	while (pathToken != NULL) {
		strcpy(tempProgram, pathToken);
		strcat(tempProgram, "/");
		strcat(tempProgram, programName);
		tempProgram[strlen(tempProgram)] = '\0';

		if (access(tempProgram, X_OK) == 0){
			strcpy(programName, tempProgram);
			return 1;
		}		
		
		pathToken = strtok(NULL, ":"); // get next token 
	}

	// if it is not in PATH environment, check on current working directory	
	currentPath = getcwd(currentPathBuffer, BUFFER_SIZE); // get current path	
	if (currentPath == NULL) {
		fprintf(stderr, "Error: Reading current working directory unsuccessful!\n");

	} else {
		strcpy(tempProgram, currentPath);
		strcat(tempProgram, "/");
		strcat(tempProgram, programName);
		tempProgram[strlen(tempProgram)] = '\0';

		if (access(tempProgram, X_OK) == 0){
			strcpy(programName, tempProgram);
			return 1;
		}
	}

	return -1; // program not found
}

/*
 * convert the path of the cd command to absolute path, if it is relative path based on the current working directory
 */
void convertToAbsolutePath(char path[]){

	char * currentPath; // pointer to current path
	char currentPathBuffer[BUFFER_SIZE];
	char tempPath[BUFFER_SIZE]; // temp path

	if (path[0] != '/') { // it is a absolute path, no need to convert					
		currentPath = getcwd(currentPathBuffer, BUFFER_SIZE); // get current path

		if (currentPath == NULL) {
			fprintf(stderr, "Error: Reading current working directory unsuccessful!\n");

		} else {			
			strcpy(tempPath, currentPath);
			strcat(tempPath, "/");
			strcat(tempPath, path);
			tempPath[strlen(tempPath)] = '\0';

			strcpy(path, tempPath); // copy back to the path name
		}
	}
}

/*
 * parse a command buffer as a program
 * output: program struct
 */
void setProgramAndArguments(char *commandBuffer, struct Program *program){
	program->numArguments = 0; // initially the number of arguments of the program is 0

	char * commandToken = strtok(commandBuffer, " "); // first string is the program name
	strcpy(program->name, commandToken); //copy program name

	// allocate the memory for argument
	// copy first argument(which is the program name)
	program->arguments[program->numArguments] = (char*)malloc(BUFFER_SIZE*sizeof(char));
	strcpy(program->arguments[program->numArguments], commandToken);
	program->numArguments++;

	commandToken = strtok (NULL, " "); // next command token
	while (commandToken != NULL)
	{
		//allocate the memory for argument and copy argument
		program->arguments[program->numArguments] = (char*)malloc(BUFFER_SIZE*sizeof(char));
		strcpy(program->arguments[program->numArguments], commandToken);
		program->numArguments++;

		commandToken = strtok (NULL, " "); // next command token
	}

	// set last argument as NULL
	program->arguments[program->numArguments] = NULL;
	program->numArguments++;
}
