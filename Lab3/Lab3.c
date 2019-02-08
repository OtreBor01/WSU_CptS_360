//Page 137 Chapter 3.11
/* 1. Prompt the user for an input line, which is of the form "cmd arg1 arg2 arg3 .... argn"
	 - Validcommands include "cd", "exit" and ANY Linux executable files, e.g. echo, ls, date, pwd, cat, cp, mv, cc, emacs, etc. 
	 - In short, the sh simulator can run the same set of commands as Linux sh.
*/
/* 2. Handle simple commands:
	 - cmd = "cd" : chdir(arg1) OR chdir(HOME) if no arg1;
	 - (DONE) cmd = "exit" : exit(0) to terminate;
*/
/* 3. For all other commands:
	 - fork a child process;
	 - wait for the child to terminate;
	 - print child’s exit status code
	 - continue step 1;
*/
/* 4. Child process: First, assume NO pipe in command line:
	 - 4-1. Handle I/O redirection:
		- cmd arg1 arg2 ... < infile // take inputs from infile
		- cmd arg1 arg2 ... > outfile // send outputs to outfile
		- cmd arg1 arg2 ... >> outfile // APPEND outputs to outfile	
	 - 4-2. Execute cmd by execve(), passing parameters
		- char *myargv[ ], char *env[ ] to the cmd file, 
		- where myargv[ ] is an array of char * 
		- with myargv[0]->cmd, myargv[1]->arg1, ....., End with a NULL pointer
	 	- Sh searches for executable commands in the directories of the PATH environment variable. 
	 	- The sh simulator must do the same. 
	 	- Thus, the simulator program must tokenize the PATH variable into individual directories for locating the command files.
*/
/* 5. After verifying mysh works for simple commands, extend it to handle pipes. 
	 - If the command line has a | symbol, divide it into head and tail: e.g.
	 	- cmd1 < infile | cmd 2 > outfile
	 	- head = "cmd < infile"; tail = "cmd 2 > outfile"
	 - Then implement the pipe by the following steps.
	 	- create a pipe
	 	- fork a child process to share the pipe
	 	- arrange one process as the pipe writer, and the other one as the pipe reader.
	 - Then let each process execve() its command (possibly with I/O redirection).
*/
/* 6. Multiple pipes: 
	 - If the command line contains multiple pipe symbols, implement the pipes by recursion. 
*/
//***** Include Recourses *****
#include <unistd.h> //For: execve()
#include "procs.c"
//***** Defined Constants *****
#define INPUT 100
#define PATH 200
#define DIRS 50
//***** Global Variables *****
char *GLOBAL_ENV[] = {NULL};
char *HOME_PATH = NULL;

//***** Function Declaration *****
char* removeWhiteSpace(char *);
void getUserInput(char*);
void getPathAndHome(char**, char*, char*);
void getCmdAndArgs(char*, char*, char*);
void getHeadTail(char*, char*, char*);
void getArguments(char *, char **);
void getDirectoryStrings(char *, char **);
void printDirectoryStrings(char **);


char* removeWhiteSpace(char *str){
	const int size = strlen(str);
	char *str2 = (char*) malloc(sizeof(char*)*size);
	bool prevSpace = false; 
	for(int i = 0, j = 0; i < size; i++){
		if(str[i] == ' '){ //character is a space
			if(!prevSpace && (i != 0)){ //not two spaces in a row and not first letter
				str2[j++] = str[i];
			}
			prevSpace = true;
		}
		else { //character is not a space
			str2[j++] = str[i];
			prevSpace = false; 
		}
	}
	if(prevSpace){ str2[strlen(str2)-1] = '\0'; }//removes trailing whitespace 
	return str2;
}

void getUserInput(char* input){
	printf("kcsh %%: ");
	fgets(input, INPUT, stdin);
	input[strlen(input)-1] = 0; // kill \n at end of input
}

void getPathAndHome(char** env, char* path, char* home){//PWD  -> env[21]
	for(int i = 0; env[i]; i++){
		if(strstr(env[i], "PATH=/")){
			strcpy(path, env[i]);
		}
		else if(strstr(env[i], "HOME=/")){
			strcpy(home, env[i]);
			HOME_PATH = (char*) malloc(sizeof(char*)*(strlen(env[i])));
			strcpy(HOME_PATH, env[i]);
		}
	}
}

void getCmdAndArgs(char* line, char* cmd, char* args){
	char *lineCopy = (char*) malloc(sizeof(char*)*(strlen(line)));
	strcpy(lineCopy, line);
	char * token = strtok(lineCopy, " ");
	strcpy(cmd, token);
	token = line + strlen(token);
	strcpy(args, token);
}

void getHeadAndTail(char* line, char* head, char* tail){
	char *lineCopy = (char*) malloc(sizeof(char*)*(strlen(line)));
	strcpy(lineCopy, line);
	char * token = strtok(lineCopy, "|");
	strcpy(head, token);
	token = strtok(NULL, "|");
	strcpy(tail, token);
}

void getArguments(char *arguments, char **myargv){
	char *argsCopy = (char*) malloc(sizeof(char*)*(strlen(arguments)));
	strcpy(argsCopy, arguments);
	char *token = strtok(argsCopy, " ");
	int i = 1;
	for(; token; i++){
		myargv[i] = (char*) malloc(sizeof(char*)*(strlen(token)));
		strcpy(myargv[i], token);
		token = strtok(NULL, " ");
	}
	myargv[i] = NULL;
}

void getDirectoryStrings(char *path, char **directoryStrings){
	char *pathCopy = (char*) malloc(sizeof(char*)*(strlen(path)));
	strcpy(pathCopy, path);
	char *token = strtok(pathCopy, ":");
	for(int i = 0; token; i++){
		if(!i){token = token + 5;} //remove "PATH=" from directory string
		directoryStrings[i] = (char*) malloc(sizeof(char*)*(strlen(token)));
		GLOBAL_ENV[i] = (char*) malloc(sizeof(char*)*(strlen(token)));
		strcpy(directoryStrings[i], token);
		strcpy(GLOBAL_ENV[i], token);
		token = strtok(NULL, ":");
	} 
}

void printDirectoryStrings(char **directoryStrings){
	puts("Decompose Path into Directory Strings:");
	for(int i = 0; directoryStrings[i]; i++){
		printf("  * Directory %d: '%s'\n", i+1, directoryStrings[i]);
	}
}

int body(char *myname)// process body function
{
  //return 0;
  int c;
  char cmd[64];
  //printf("proc %d starts from body(): myname = %s\n", running->pid, myname);
  while(1){
  /*
    printf("***************************************\n");
    printf("proc %d running: parent=%d  ", running->pid, running->ppid);
    printChild("childList", running->child);
    printList("readyQueue", readyQueue);
    printSleep("sleepList ", sleepList);
    //**********************************
    */
	puts("*********** kcsh processing loop ***********");
	//***** User Input *****
	char input[INPUT] = "";
	getUserInput(input); //(1) Prompt the user for an input line, which is of the form "cmd arg1 arg2 arg3 .... argn" 
	//***** Command & Arguments *****
	char command[INPUT] = "", arguments[INPUT] = "";
 	getCmdAndArgs(input, command, arguments);
 	strcpy(arguments, removeWhiteSpace(arguments));
	//***** myargv & GLOBAL_ENV *****
 	char *myargv[] = {command};
 	getArguments(arguments, myargv);
 	puts("Arguments:");
	for(int i = 0; myargv[i]; i++){
		printf("  * Arg %d: '%s'\n", i+1, myargv[i]);
	}
	//***** Path (Argument 1) *****
	char *path = ((myargv[1] != NULL) && (strlen(myargv[1]) > 0)) ? myargv[1] : HOME_PATH; // chdir(arg1) OR chdir(HOME) if no arg1;
	//***** Head & Tail*****
	char head[INPUT] = "", tail[INPUT] = "";//(5) If the command line has a | symbol, divide it into head and tail
	if(strstr(arguments, "|")){ getHeadAndTail(arguments, head, tail); }//populates head and tail if '|' exist in arguments
	//***** Simple Commands (exit & cd)*****
	if(!strcmp(command, "exit")){ return 0; } // cmd = "exit" : exit(0) to terminate;
	if(!strcmp(command, "cd")){ printf("PROC %d cd to '%s' was %s\n", running->pid, path, (!chdir(path))? "Valid": "Invalid"); }//cmd = "cd" : Success = 0, Error = -1 is returned, and errno is set appropriately.
	//***** Linux Executable Files *****
	else{
		do_kfork(); // fork a child process;
		do_wait(); // wait for the child to terminate;
		// print child’s exit status code
		// continue step 1;
	}
	int result = execve(path, myargv, GLOBAL_ENV); // int execve(const char *filename, char *const argv[], char *const envp[]);

	/**** Proccess Command Options *****
    if (strcmp(command, "ps")==0)
      do_ps();
    if (strcmp(command, "fork")==0)
      do_kfork();
    if (strcmp(command, "switch")==0)
      do_switch();
    if (strcmp(command, "exit")==0)
      do_exit();
    if (strcmp(command, "jesus")==0)
      do_jesus();
   	if (strcmp(command, "sleep")==0)
      do_ksleep();
   	if (strcmp(command, "wakeup")==0)
      do_wakeup();
   	if (strcmp(command, "wait")==0)
      do_wait();
   	if (cmd == NULL)
   	  return do_exit();
   	*/
 }
}

int main(int argc, char *argv[], char *env[]){
	char path[PATH], home[PATH];
	getPathAndHome(env, path, home); //gets the path and home values from env
	puts("************* Welcome to kcsh **************");
	printf("Home Directory: %s\n", home);
	printf("Path: %s\n", path);
	char *directoryStrings[DIRS] = {NULL};
	getDirectoryStrings(path, directoryStrings);
	printDirectoryStrings(directoryStrings);
	init();    // initialize system; create and run P0
    kfork();   // kfork P1 into readyQueue  
    while(1){
    	//printf("P0: switch process\n");
     	while (readyQueue == 0); // loop if readyQueue empty
     	tswitch();
   	}
	return 0;
}
