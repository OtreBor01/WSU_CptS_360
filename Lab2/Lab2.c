/*
mkdir pathname :make a new directory for a given pathname
rmdir pathname :remove the directory, if it is empty.
cd [pathname] :change CWD to pathname, or to / if no pathname.
ls [pathname] :list the directory contents of pathname or CWD
pwd :print the (absolute) pathname of CWD
creat pathname :create a FILE node.
rm pathname :remove the FILE node.
save filename :save the current file system tree as a file
reload filename :construct a file system tree from a file
menu : show a menu of valid commands
quit : save the file system tree, then terminate the program.
*/
//Libraries neccesary include 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <libgen.h>
#include <ctype.h>
//Defined Constants

//Custom Data Structures and Types 
typedef struct node{
	char Name[64];
	char Type;
	struct node *Child, *Sibling, *Parent;
}Node;

//Function Declarations
Node * createNode(char*, char);
void dbname(char *); bool tokenize(char *);
Node * locateDir(char*); void initialize();
int findCmd(char *); int mkdir(char *);
int rmdir(char *);   int ls(char *);
int cd(char *);      int pwd(char *);
int creat(char *);   int rm(char *);
int reload(char *);  int save(char *);
int menu(char *);    int quit(char *);
//Gloabal Variables
Node *root, *cwd; // root and CWD (Current Working Directory) pointers
char line[128]; // user input command line
char command[16], pathname[64] = "ROOT"; // command and pathname strings
char dname[64], bname[64]; // dirname and basename string holders
char *cmd[] = {"mkdir", "rmdir", "ls", "cd", "pwd", "creat", "rm",
			   "reload", "save", "menu", "quit", NULL};
int (*fptr[ ])(char *)={(int (*)())mkdir,rmdir,ls,cd,pwd,creat,rm,
									reload,save,menu,quit};

int main(void){
	initialize(); //initialize root node of the file system tree
	while(true){
		char path[64];
		printf("%s$ ", pathname);
		fgets(line, 128, stdin); //get user input line = [command pathname];
		line[strlen(line)-1] = 0; // kill \n at end of line
		sscanf(line, "%s %s", command, path); //identify the command & path name
		if(!strcmp(command, "quit")){
			break;
		}
		int index = findCmd(command); //finds the command to execute
		if(index == -1){
			printf("%s: command not found\n", command);
			continue;
		}
		//printf("Command: %s\nPath: %s\nIndex: %d\n", command, pathname, index);
		dbname(path);
		//printf("dname: %s\tbname: %s\n", dname, bname);
		if(!tokenize(dname)){ //attempts to change the cwd
			printf("bash: %s: %s: No such file or directory\n", command, path);
			continue;
		}
		int r = fptr[index](bname); //executes the command
	}
	
	return 0;
}
void initialize(){
	//Possibly load from saved file
	root = (Node*) malloc(sizeof(Node));
	root->Parent = NULL;
	root->Sibling = NULL;
	root->Type = 'D';
	strcpy(root->Name, pathname);
	cwd = root;
}
Node * locateDir(char *name){
	Node *child = cwd->Child;
	while(child){
		if(child->Type == 'D' && !strcmp(child->Name, name)){
			return child;
		}
		child = child->Sibling;
	}
	return NULL;
}
bool tokenize(char *path)
{
	char* delimiter = "/", tempPath[64];
	strcpy(tempPath, pathname);
	char *token = strtok(path, delimiter); // first call to strtok()
	Node * cur = cwd;
	while(token){
		if(!strcmp(token, "..") && cwd != root){
			cur = cur->Parent;
			char * substr = strrchr(tempPath, '/');
			int n = strlen(tempPath)-strlen(substr);
			tempPath[n] = '\0';
		}
		else if((cur =locateDir(token)) == NULL){ //If invalid directory name
			return false;
		}
		else{
			strcat(tempPath, "/");
			strcat(tempPath, cur->Name);
		}
		token = strtok(NULL, delimiter); // call strtok() until it returns NULL
	}
	cwd = cur;
	strcpy(pathname, tempPath);
	return true;
}
void dbname(char *path){
	char temp[128]; // dirname(), basename() destroy original pathname
	strcpy(temp, path);
	strcpy(dname, dirname(temp));
	strcpy(temp, path);
	strcpy(bname, basename(temp));
	if(!strcmp(dname, ".")){strcpy(dname, "");}
}
int findCmd(char *command)
{
	int i = 0;
	while(cmd[i]){
		if (!strcmp(command, cmd[i])) {
			return i; // found command: return index i
		} i++;
	}
	return -1; // not found: return -1
}
Node * createNode(char* name, char type){
	Node *newNode = (Node *) malloc(sizeof(Node));
	strcpy(newNode->Name, name);
	newNode->Type = type;
	newNode->Sibling = newNode->Parent = NULL;
	return newNode;
}
int mkdir(char *name){
	Node *cur = cwd;
	while(cur){
		if(cur->Type == 'D' && !strcmp(cur->Name, name)){
			printf("mkdir: cannot create directory '%s': File exists\n", name);
			return false;
		}
		cur = cur->Sibling;
	}
	Node *newDir = createNode(name, 'D');
	newDir->Sibling = cwd->Child ? cwd->Child : NULL;
	newDir->Parent = cwd;
	cwd->Child = newDir;
	return true;
}
int rmdir(char *name){
	Node *prev = NULL, *cur = cwd;
	while(cur){
		if(cur->Type == 'D' && !strcmp(cur->Name, name)){
			if(cur == cwd->Child){ cwd->Child = cur->Sibling; }
			else if(prev != NULL){ prev->Sibling = cur->Sibling; }
			free(cur);
			return true;
		}
		prev = cur;
		cur = cur->Sibling;
	}
	printf("rmdir: failed to remove '%s': No such file or directory\n", name);
	return false;
}   
int ls(char *name){
	Node *sibling = cwd->Child;
	//bool displayAll = (name == NULL || !strcmp(name, "")) ? true : false;
	for(int i = 1; sibling; i++){
		if(true){
			if(i!=5){printf("%s (%c)\t", sibling->Name, sibling->Type);}
			else{ i==0; printf("\n%s (%c)", sibling->Name, sibling->Type);}
		}
		sibling = sibling->Sibling;
	}puts("");
	return true;
}
int cd(char *name){
	return tokenize(name);
	/*Node *cur = cwd;
	while(cur){
		if(cur->Type == 'D' && !strcmp(cur->Name, name)){
			cwd = cur->Child;
			return true;
		}
		cur = cur->Sibling;
	}
	return false;
	*/
}
int pwd(char *name){
	/*Start from CWD, implement pwd by recursion:
(1). Save the name (string) of the current node
(2). Follow parentPtr to the parent node until the root node;
(3). Print the names by adding / to each name string
	*/
}
int creat(char *name){
	//SAME AS mkdir except the node type is ‘F’
}   
int rm(char *name){
	//SAME AS rmdir except check it's a file, no need to check for EMPTY.
}
int reload(char *name){
/*
The function reconstructs a tree from a file. First, initialize the tree as empty,
i.e. with only the root node. Then read each line of the file. If the line contains “D pathname”, call
mkdir(pathname) to make a directory.
If the line contains “F pathname”, call
creat(pathname) to create a file.
These will reconstruct the tree saved earlier.
*/
}	
int save(char *name){
	//Check Textbook (pg. 95-96)
}
int menu(char *name){
	
}
int quit(char *name){
	/*
save the current tree to a file. Then terminate the program execution. 
On subsequent runs of the simulator program, the user may use the reload command to 		restore the tree saved earlier.
	*/
}


