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
char *getBaseName(char *); char *getDirName(char *);
Node *createNode(char*, char); Node *locateDir2(char *);
void dbname(char*); Node *locateDir(char *);
Node * traverseDir(char*); void initialize();
int findCmd(char *); int mkdir(char *);
int rmdir(char *);   int ls(char *);
int cd(char *);      int pwd(char *);
int creat(char *);   int rm(char *);
int reload(char *);  int save(char *);
int menu(char *);    int quit(char *);
void displayNode(Node *);
//Gloabal Variables
Node *root, *cwd, *start; // root and CWD (Current Working Directory) pointers
char pathname[64] = "~", cwPath[64] = "",  dname[64] = "", bname[64] = "";
char *cmd[] = {"mkdir", "rmdir", "ls", "cd", "pwd", "creat", "rm",
			   "reload", "save", "menu", "quit", NULL};
int (*fptr[ ])(char *)={(int (*)())mkdir,rmdir,ls,cd,pwd,creat,rm,
									reload,save,menu,quit};

int main(void){
	initialize(); //initialize root node of the file system tree
	while(true){
		cwd = start;
		strcpy(cwPath, pathname);
		char line[128] = "", path[64] = "", command[16] = "";
		int index = 0; //used for command index
		strcpy(dname, ""); strcpy(bname, ""); 
		printf("%s$ ", pathname);
		fgets(line, 128, stdin); //get user input line = [command pathname];
		line[strlen(line)-1] = 0; // kill \n at end of line
		sscanf(line, "%s %s", command, path); //identify the command & path name
		if(strlen(path)){ dbname(path);}
		printf("path: %s\tdname: %s\tbname: %s\n", path, dname, bname);
		if(!strcmp(command, "quit")){
			break;
		}
		else if((index = findCmd(command)) == -1){//finds the command to execute
			printf("%s: command not found\n", command);
			continue;
		}
		else if((cwd = locateDir(dname)) == NULL){ //attempts to find the specified path
			printf("bash: %s: %s: No such file or directory\n", command, path);
			continue;
		}
		printf("path: %s\tdname: %s\tbname: %s\n", path, dname, bname);
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
	start = cwd = root;
}
char *getBaseName(char *path){
	bool isDir = false;
	char *temp = path; 
	for(int i = 0; *temp != '\0'; i++){
		if(*temp == '/'){ isDir = true; break;}
		temp++;
	}
	if(isDir){return strrchr(path, '/');}
	return path;
}
char *getDirName(char *path){
	int index = -1;
	char *temp = path; 
	for(int i = 0; *temp != '\0'; i++){
		if(*temp == '/'){ index = i;}
		temp++;
	}
	if(index != -1){return strncpy(temp, path, index);}
	else {strcpy(temp, "");}
	return temp;
}
Node * traverseDir(char *name){
	Node *child = cwd->Child;
	while(child){
		if(child->Type == 'D' && !strcmp(child->Name, name)){
			return child;
		}
		child = child->Sibling;
	}
	return NULL;
}
Node * locateDir2(char *path)
{
	char* delimiter = "/";
	char *token = strtok(path, delimiter); // first call to strtok()
	Node *cur = cwd;
	while(token){
		puts(token);
		if(!strcmp(token, "..") && cwd != root){
			cur = cur->Parent;
		}
		else if((cur=traverseDir(token)) == NULL){ //If invalid directory name
			return NULL;
		}
		token = strtok(NULL, delimiter); // call strtok() until it returns NULL
	}
	return cur;
}
Node *locateDir(char *path)
{
	char* delimiter = "/";
	char *token = strtok(path, delimiter); // first call to strtok()
	Node * cur = cwd;
	while(token){
		printf("Token: '%s'\n", token);
		if(!strcmp(token, "..") && cwd != root){
			cur = cur->Parent;
			char * substr = getBaseName(cwPath);
			int n = strlen(cwPath)-strlen(substr);
			cwPath[n] = '\0';
		}
		else if((cur=traverseDir(token)) == NULL){ //If invalid directory name
			return false;
		}
		else{
			strcat(cwPath, "/");
			strcat(cwPath, cur->Name);
		}
		token = strtok(NULL, delimiter); // call strtok() until it returns NULL
	}
	return cur;
}
void dbname(char *path){
		char temp[128]; // dirname(), basename() destroy original pathname
		strcpy(temp, path);
		strcpy(dname, dirname(temp));
		strcpy(temp, path);
		strcpy(bname, basename(temp));
		strcpy(path, "");
		//set bname or dname to the empty string if they are equal to '.'
		if(strlen(dname) == 1 && (*dname) == '.'){strcpy(dname, ""); }
		else{ strcat(path, dname); strcat(path, "/"); }
		if(strlen(bname) == 1 && (*bname) == '.'){strcpy(bname, ""); }
		else { strcat(path, bname); }
}
int findCmd(char *command)
{
	int i = 0;
	while(cmd[i]){
		if (!strcmp(command, cmd[i])) {
			return i; // found command: return index i
		} i++;
	}
	return -1; // not found: ccdreturn -1
}
Node * createNode(char* name, char type){
	Node *newNode = (Node *) malloc(sizeof(Node));
	strcpy(newNode->Name, name);
	newNode->Type = type;
	newNode->Sibling = newNode->Parent = newNode->Child = NULL;
	return newNode;
}
void displayNode(Node *node){
	puts("***** Node *****");
	printf("Name: %s\nType: %c\n", node->Name, node->Type);
	char *parent = node->Parent ? node->Parent->Name : "N/A";
	char *sibling = node->Sibling ? node->Sibling->Name : "N/A";
	char *child = node->Child ? node->Child->Name : "N/A";
	printf("Parent: %s\nSibling: %s\nChild: %s\n", parent, sibling, child);
}
int mkdir(char *name){
	Node *prev = NULL, *cur = cwd->Child;
	while(cur){
		if(cur->Type == 'D' && !strcmp(cur->Name, name)){
			printf("mkdir: cannot create directory '%s': File exists\n", name);
			return false;
		}prev = cur;
		cur = cur->Sibling;
	}
	Node *newDir = createNode(name, 'D');
	newDir->Parent = cwd;
	if(prev) {prev->Sibling = newDir;}
	else {cwd->Child = newDir;}
	return true;
}
int rmdir(char *name){
	Node *prev = NULL, *cur = cwd->Child;
	while(cur){
		if(cur->Type == 'D' && !strcmp(cur->Name, name)){
			if(cur == cwd->Child){ cwd->Child = cur->Sibling; }
			else if(prev != NULL){ prev->Sibling = cur->Sibling; }
			free(cur);
			return true;
		} prev = cur;
		cur = cur->Sibling;
	}
	printf("rmdir: failed to remove '%s': No such file or directory\n", name);
	return false;
}   
int ls(char *name){
	if(!(cwd = locateDir(name))){
		printf("ls: cannot access '%s': No such file or directory\n", name);
		return false;
	}
	if(cwd->Type == 'D'){
		Node *cur = cwd->Child;
		for(int i = 1; cur; i++){
			if(true){
				if(i!=5){printf("%s (%c)\t", cur->Name, cur->Type);}
				else{ i==0; printf("\n%s (%c)", cur->Name, cur->Type);}
			}
			cur = cur->Sibling;
		}puts("");
	} else{ printf("%s (%c)\n", cwd->Name, cwd->Type); }
	return true;
}
int cd(char *name){
	if((cwd = locateDir(bname)) != NULL){
		strcpy(pathname, cwPath);
		start = cwd;
		return true;
	}
	strcat(cwPath, "/"); strcat(cwPath, name);
	printf("bash: cd: %s: No such file or directory\n", cwPath);
	return false;
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


