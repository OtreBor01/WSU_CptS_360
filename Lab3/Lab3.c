/*
* exit [void _exit(int status)]: http://pubs.opengroup.org/onlinepubs/7908799/xsh/_exit.html
* pipe [int pipe(int fildes[2])]: http://pubs.opengroup.org/onlinepubs/7908799/xsh/pipe.html
* pthread_atfork [int pthread_atfork(void (*prepare)(void), void (*parent)(void), void (*child)(void))]: http://pubs.opengroup.org/onlinepubs/7908799/xsh/pthread_atfork.html
* fork [pid_t fork(void)]: http://pubs.opengroup.org/onlinepubs/7908799/xsh/fork.html
* kill [int kill(pid_t pid, int sig)]: http://man7.org/linux/man-pages/man2/kill.2.html
* wait [pid_t wait(int *wstatus)]: http://man7.org/linux/man-pages/man2/wait.2.html
* getenv [char *getenv(const char *name)]: http://pubs.opengroup.org/onlinepubs/009695399/functions/getenv.html 
*/
//***** Include Recourses *****
#include <unistd.h> //http://pubs.opengroup.org/onlinepubs/7908799/xsh/unistd.h.html
#include <sys/types.h> //http://pubs.opengroup.org/onlinepubs/7908799/xsh/systypes.h.html
#include <sys/wait.h>
#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

//***** Defined Constants *****
#define LINE 100
#define DIR 50
#define CHARSIZE sizeof(char*)
#define PPID getppid()
#define READ 0
#define WRITE 1
//***** Global Variables *****
char *HOME = ""; //HOME= '/home/austinmm'
char *PATH = ""; //PATH= '/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin'
char *DIRS[DIR];
char *BIN = "/bin";
char *ENV[] = { "HOME=/usr/home", "LOGNAME=home", (char *)0 };
//char *ENV[] = { "HOME=/home/austinmm", "LOGNAME=austinmm", (char *)0 };
int WSTATUS = 0;
int PIPE[4]; //3 posible pipes

//***** Function Declaration *****
int run(char**);
void envInfo(char**), getDirectories(char*);
char* getInput(void);
void argvInfo(char*,char**);
bool hasPipe(char**, char**, char**);
int handlePipes(char**,char**,int);
int handleIO(char*,char*);
bool simpleCommands(char **);
int cd(char*);
void linuxExe(char*,char**);
void forkInfo(void);
char* getCMD(char**);
//***** Helper Functions *****
void ArrtoStr(char**,char*);
char* createCopyStr(char*);
char* removeWhiteSpace(char*);
void printStrArray(char *, char **);
void cleanStrArr(char** strArr);
void removeIOArgs(char**);

char* getInput(void){
	char cwd[DIR]; getcwd(cwd, DIR);
	char line[LINE] = {0}, *str = NULL;
	printf("%s$ ", cwd);	//printf("Command: ");
    //fflush(stdout);
	if(fgets(line,LINE,stdin)){
    	char *p;
        if(p = strchr(line, '\n')){//check exist newline
            *p = 0;
        } else {
            scanf("%*[^\n]");scanf("%*c");//clear upto newline
        }
    }
	str = (char*) malloc((CHARSIZE*strlen(line)));
	strcpy(str, removeWhiteSpace(line));
	int size = strlen(str)-1;
	if(str[size] == '\n') { str[size] = 0; } // kill \n at end of line = 0
	return str;
}

void envInfo(char** env){
	for(int i = 0; env[i]; i++){
		if(strstr(env[i], "PATH=/")){
			PATH = (char*) malloc(CHARSIZE*(strlen(env[i])));
			strcpy(PATH, env[i]);
			getDirectories(env[i]);
		}
		else if(strstr(env[i], "HOME=/")){
			HOME = (char*) malloc(CHARSIZE*(strlen(env[i])));
			strcpy(HOME, env[i]);
		}
	}
}

void getDirectories(char *path){
	char *token = strtok(path, ":");
	int i = 0;
	for(; token; i++){
		if(!i){token = token + 5;} //remove "PATH=" from directory string
		DIRS[i] = (char*) malloc(CHARSIZE*(strlen(token)));
		strcpy(DIRS[i], token);
		token = strtok(NULL, ":");
	} 
	DIRS[i] = NULL;
}
 
void argvInfo(char *line, char **args){
	char *copy = createCopyStr(line);
	char *token = strtok(copy, " ");
	int i = 0, strS = 0;
	for(; token; i++){
		strS = strlen(token)+1;
		args[i] = (char*) malloc(CHARSIZE * strS);
		strcpy(args[i], token);
		token = strtok(NULL, " ");
	}
	args[i] = NULL;
}

bool hasPipe(char **args, char **head, char **tail){
	bool foundPipe = false; char *str = NULL;
	int i = 0, h_i = 0, t_i = 0;
	for(; args[i] ; i++){
		int size = strlen(args[i]) + 1;
		if(!strcmp(args[i], "|") || foundPipe){ 
			if(!foundPipe){ foundPipe = true; i++;}
			tail[t_i] = (char*) malloc(CHARSIZE*size);
			strcpy(tail[t_i++], args[i]);
		}
		else{ 
			head[h_i] = (char*) malloc(CHARSIZE*size);
			strcpy(head[h_i++], args[i]);
	 	}
	}
	head[h_i] = NULL;
	tail[t_i] = NULL;
	return foundPipe;
}

bool simpleCommands(char **args){
	//***** Simple Commands (exit & cd)*****
	bool isSimple = false;	
	if(!strcmp(args[0], "exit")){ isSimple = true; exit(EXIT_SUCCESS); } 
	else if(!strcmp(args[0], "cd")){ isSimple = true; cd(args[1]); }
	return isSimple;
}

int cd(char *path){
	if(!path || !strlen(path)){ path = getenv("HOME"); }//path is was empty
	int result = chdir(path);
	printf("PROC %d cd to '%s' was %s \n", getpid(), path, (!result) ? "Successful" : "Unsuccessful"); 
	return result;
}

void forkInfo(void){
	printf("Parent kcsh PROC %d forks a child process %d \n", getppid(), getpid()); 
	printf("Parent sh PROC %d waits \n", getppid());
	//wait: on success, returns the process ID of the terminated child;on error, -1 is returned	
}

char* getCMD(char** args){
	for(int i = 0; DIRS[i]; i++){
		char *cmd = createCopyStr(DIRS[i]);
		strcat(cmd, "/");
		strcat(cmd, args[0]);
		printf("command: '%s' | i = %d | cmd = '%s' \n", args[0], i, cmd);
		if(!strcmp(DIRS[i], BIN)){ return cmd; }
	}
	return NULL;
}

int run(char **args) //line = 'cat file.txt | more'
{
	char line[LINE] = {0};
	ArrtoStr(args, line);
	//***** Linux Executable Files *****
	if(simpleCommands(args)){ return 1; }
	char *head[10] = {0}, *tail[10] = {0};
	bool containsPipe = hasPipe(args, head, tail); //checks if args contains a pipe, '|' and assigns head and tail respectively
	int pid = fork(); //fork proccess
	if (pid < 0) { printf("fork Failed \n"); }
	else if (pid){
		int status = 0, child = 0;
		while ((status = wait(&WSTATUS)) > 0) { child = status; }
		if(child == -1){ printf("Failure to wait on child proc \n"); }
		else{ printf("child sh PROC %d died : exit status = %d \n", child, WSTATUS); }
	}
	else { // Child process
		forkInfo();
		printf("PROC %d: line = '%s' \n", getpid(), line);
		if(containsPipe) { //pipe exist	
			return handlePipes(head, tail, 0);
		}
		else{ //No Pipe
			printf("No Pipe: Head = '%s', Tail = 'NULL' \n", line);
			linuxExe(line, args); 
		    return -1;
		}
		exit(getpid());
	}
	return 1;
}

int handlePipes(char **head, char **tail, int pCount){
	char hLine[DIR] = {0}, tLine[DIR] = {0};
	ArrtoStr(head, hLine);
	ArrtoStr(tail, tLine);
	printf("Has Pipe: Head = '%s', Tail = '%s' \n", hLine, tLine);
	int *pd = &PIPE[pCount];
	if (pipe(pd) == -1){ printf("Pipe Failed \n" ); } 
	int pid = fork(); //fork a child (to share the PIPE)
	if (pid < 0) { printf("Pipe-Fork Failed \n"); }
	if (pid){ //parent as pipe WRITER
		close(pd[READ]); //WRITER MUST close pd[0]
		close(1); //close 1
		dup(pd[WRITE]); //replace 1 with pd[1]
		close(pd[WRITE]); //close pd[1]
		linuxExe(hLine, head); //change image to head cmd
		return -1;  
	}
	else{ //child as pipe READER
		close(pd[WRITE]); //READER MUST close pd[1]
		close(0); //close 0
		dup(pd[READ]); //replace 0 with pd[0]
		close(pd[READ]);//close pd[0]
		char *h[10] = {0}, *t[10] = {0};
		if(hasPipe(tail, h, t)){ 
			return handlePipes(h, t, pCount+1); }
		else{
			linuxExe(tLine, tail);
			return -1;  
		}
	}
	return 0;
}

/*
int handlePipes(char **head, char **tail){
	int pd[2];
	char hLine[DIR] = {0}, tLine[DIR] = {0};
	ArrtoStr(head, hLine);
	ArrtoStr(tail, tLine);
	printf("Has Pipe: Head = '%s', Tail = '%s' \n", hLine, tLine);
	if (pipe(pd) == -1){ printf("Pipe Failed \n" ); } 
	int pid = fork();
	if (pid < 0) { printf("Pipe-Fork Failed \n"); }
	else if(pid){//parent as pipe writer
		parentPipe(pd);
		linuxExe(hLine, head);  
	}else{ //child as pipe reader
		char *h[10] = {0}, *t[10] = {0};
		childPipe(pd);
		if(hasPipe(tail, h, t)){ puts("Enbeded Pipes"); return handlePipes(h,t); }
		else{
			linuxExe(tLine, tail);
			return -1;  
		}
	}
	return 1;
}
*/
int handleIO(char *file, char *line){
    if((file = memchr(line,'<',LINE))){ // take inputs from infile
    	close(0);
    	file = removeWhiteSpace(file+1);
    	return open(file, O_RDONLY);
	}
    else if((file = strstr(line,">>"))) { // APPEND outputs to outfile
    	close(1);
    	file = removeWhiteSpace(file+2);
        return open(file, O_RDWR|O_APPEND, 0644);
    }
    else if((file = memchr(line,'>',LINE))){ // send outputs to outfile
    	close(1);
    	file = removeWhiteSpace(file+1);
        return open(file, O_WRONLY|O_CREAT, 0644);
	}
    return -1; //No IO Redirect
}

void linuxExe(char *line, char **args){
	char *command = args[0];
	printf("PROC %d do_command: line = '%s' \n", getpid(), line);
	printStrArray("", args);
	printf("PROC %d tries %s in each PATH dir \n", getpid(), command);
	char *file = NULL;
	char *cmd = getCMD(args);
	if (handleIO(file, line) != -1) //Checks for IO Redirect, enters conditional if has one
	{
		removeIOArgs(args);       		
	}
	execve(cmd, args, ENV);
	printf("Failed to Execute Command:'%s' \n", command);
}

int main(int argc, char *argv[], char *env[]){
	envInfo(env); // gets the HOME,ENV, and DIR values from env
	printf("************* Welcome to kcsh ************** \n");
	printf("Parent Processor ID: %d \n", PPID);
	printf("Home Directory: %s \n", HOME);
	printf("Path: %s\n", PATH);
	printf("Decompose Path into Directory Strings: \n");
    printStrArray("  * Dir", DIRS);
	printf("*********** kcsh processing loop *********** \n");
	char line[LINE] = {0};
	while(true){
		strcpy(line, getInput()); //(1) Prompt the user for an input line, which is of the form "cmd arg1 arg2 arg3 .... argn" 
		//***** arguments *****
		char *args[20] = {0};
		argvInfo(line, args); //args = { "cat", "file.txt", "|", "more", NULL }
		int result = run(args);
	}
	return 0;
}
void ArrtoStr(char**arr, char*str){
	strcpy(str, "");
	for(int i = 0; arr[i]; i++){
		strcat(str, arr[i]);
		if(arr[i+1]){ strcat(str, " "); }
	}
}
char* createCopyStr(char *str){
	char *str2 = (char*) malloc(CHARSIZE*(strlen(str)));
	strcpy(str2, str);
	return str2;
}

void printStrArray(char *name, char **arr){
	for(int i = 0; arr[i]; i++){
		printf("%s %d: '%s' \n", name, i, arr[i]);
	}
}

char* removeWhiteSpace(char *str){
	const int size = strlen(str);
	char *str2 = (char*) malloc((CHARSIZE*size)+1);
	bool prevSpace = false; 
	for(int i = 0, j = 0; i < size; i++){ //character is a space
		if(str[i] == ' '){
			if(!prevSpace && (i != 0)){ //not two spaces in a row and not first letter
				str2[j++] = str[i];
			}
			prevSpace = true;
		}
		else{ //character is not a space or '\n'
			str2[j++] = str[i];
			prevSpace = false; 
		}
	}
	str2[strlen(str2)] = '\0'; //removes trailing whitespace 
	return str2;
}
void cleanStrArr(char** strArr)
{
    //clear the arguments from the past run call
    int i = 0;
    for (; strArr[i]; i++)
    {
        memset(strArr[i], 0, strlen(strArr[i]));     
    }
 	strArr[i] = NULL;
}
void removeIOArgs(char **args){
	for(int i = 0; args[i]; i++){
		char *s = args[i];
		if(!strcmp(s, ">") || !strcmp(s, "<") || !strcmp(s, ">>")){
			args[i] = NULL;
			args[++i] = NULL;
		}
	}
}
