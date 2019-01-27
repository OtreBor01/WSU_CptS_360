/*
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
}*/
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
char *getBaseName(char*);       char *getDirName(char*);
Node *createNode(char*,char);   char *getPath(Node*);
void dbname(char*); 			Node *locateDir(char*);
Node * traverseDir(char*); 		void initialize();
int findCmd(char*); 			int mkdir(char*);
int rmdir(char*);   			int ls(char*);
int cd(char*);      			int pwd(char*);
int creat(char*);   			int rm(char*);
int reload(char*);  			int save(char*);
int menu(char*);    			int quit(char*);
void displayNode(Node*); 		int runCommand(char*,char*);
void saveTree(FILE*,Node*);
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
		strcpy(dname, ""); strcpy(bname, ""); 
		printf("%s$ ", pathname);
		fgets(line, 128, stdin); //get user input line = [command pathname];
		line[strlen(line)-1] = 0; // kill \n at end of line
		sscanf(line, "%s %s", command, path); //identify the command & path name
		int r = runCommand(command, path);
		if(r == -1){ break; }
	}
	return 0;
}
int runCommand(char *command, char *path){
	int index = 0; //used for command index
	if(strlen(path)){ dbname(path);}
	if(!strcmp(command, "quit")){
		int r = quit("");
		return -1;
	}
	else if((index = findCmd(command)) == -1){//finds the command to execute
		printf("%s: command not found\n", command);
		return 0;
	}
	else if((cwd = locateDir(dname)) == NULL){ //attempts to find the specified path
		printf("bash: %s: %s: No such file or directory\n", command, path);
		return 0;
	}
	//printf("path: %s\tdname: %s\tbname: %s\n", path, dname, bname);
	return fptr[index](bname); //executes the command
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
Node *locateDir(char *path)
{
	char* delimiter = "/";
	char *token = strtok(path, delimiter); // first call to strtok()
	Node * cur = cwd;
	while(token){
		//printf("Token: '%s'\n", token);
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
//mkdir pathname :make a new directory for a given pathname
int mkdir(char *name){
	Node *prev = NULL, *cur = cwd->Child;
	while(cur){
		if(cur->Type == 'D' && !strcmp(cur->Name, name)){
			printf("mkdir: cannot create directory '%s': Directory exists\n", name);
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
//rmdir pathname :remove the directory, if it is empty.
int rmdir(char *name){
	Node *prev = NULL, *cur = cwd->Child;
	while(cur){
		if(cur->Type == 'D' && !strcmp(cur->Name, name)){
			if(cur->Child != NULL){
				printf("rmdir: failed to remove '%s': Directory not empty\n", name);
				return false;
			}
			else if(cur == cwd->Child){ cwd->Child = cur->Sibling; }
			else if(prev != NULL){ prev->Sibling = cur->Sibling; }
			free(cur);
			return true;
		} prev = cur;
		cur = cur->Sibling;
	}
	printf("rmdir: failed to remove '%s': No such directory exist\n", name);
	return false;
}
//ls [pathname] :list the directory contents of pathname or CWD  
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
//cd [pathname] :change CWD to pathname, or to / if no pathname.
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
//pwd :print the (absolute) pathname of CWD
int pwd(char *name){
	puts(getPath(cwd));
	return true;
}
char *getPath(Node *node){
	char *name = node->Name;
	if((node = node->Parent) == NULL){ 
		return "/"; 
	}
	char *parentName = getPath(node);
	if(strcmp(parentName, "/")){
		strcat(parentName, "/"); 
	}
	strcat(parentName, name);
	return parentName;
}
//creat pathname :create a FILE node.
int creat(char *name){
	Node *prev = NULL, *cur = cwd->Child;
	while(cur){
		if(cur->Type == 'F' && !strcmp(cur->Name, name)){
			printf("creat: cannot create file '%s': File exists\n", name);
			return false;
		}prev = cur;
		cur = cur->Sibling;
	}
	Node *newDir = createNode(name, 'F');
	newDir->Parent = cwd;
	if(prev) {prev->Sibling = newDir;}
	else {cwd->Child = newDir;}
	return true;
}
//rm pathname :remove the FILE node.   
int rm(char *name){
	Node *prev = NULL, *cur = cwd->Child;
	while(cur){
		if(cur->Type == 'F' && !strcmp(cur->Name, name)){
			if(cur == cwd->Child){ cwd->Child = cur->Sibling; }
			else if(prev != NULL){ prev->Sibling = cur->Sibling; }
			free(cur);
			return true;
		} prev = cur;
		cur = cur->Sibling;
	}
	printf("rm: failed to remove '%s': No such file\n", name);
	return false;
}
//reload filename :construct a file system tree from a file
int reload(char *name){
	FILE *fp = fopen(name, "r"); // fopen a FILE stream for reading
	char line[64];
	initialize();
	while(fgets(line, 64, fp)){
		char *type, *path = "", *command = "mkdir";
		sscanf(line, "%s %s", type, path); // print a line to file
		if(type[0] == 'F'){ command = "creat"; }
		int r = runCommand(command, path);
	}
	fclose(fp); // close FILE stream when done
/*
The function reconstructs a tree from a file. First, initialize the tree as empty,
i.e. with only the root node. Then read each line of the file. If the line contains “D pathname”, call
mkdir(pathname) to make a directory.
If the line contains “F pathname”, call
creat(pathname) to create a file.
These will reconstruct the tree saved earlier.
*/
}
//save filename :save the current file system tree as a file
int save(char *name){
	FILE *fp = fopen(name, "w"); // fopen a FILE stream for WRITE
	saveTree(fp, root->Child);
	fclose(fp); // close FILE stream when done
}
void saveTree(FILE * fp, Node * node){
	if(node){
		fprintf(fp, "%c %s\n", node->Type, getPath(node)); // print a line to file
		if(node->Child){ saveTree(fp, node->Child); }
		if(node->Sibling){ saveTree(fp, node->Sibling); }
	}
}
//menu : show a menu of valid commands
int menu(char *name){
	puts("mkdir  <pathname> : makes a new directory for a given pathname.");
	puts("rmdir  <pathname> : removes the directory, if it is empty.");
	puts("ls     <pathname> : lists the directory contents of pathname or CWD.");
	puts("cd     <pathname> : changes CWD to pathname, or to / if no pathname."); 
	puts("creat  <pathname> : creates a FILE node.");
	puts("rm     <pathname> : removes the FILE node.");
	puts("save   <filename> : saves the current file system tree as a file.");
	puts("reload <filename> : constructs a file system tree from a file.");
	puts("pwd  : prints the (absolute) pathname of CWD.");
	puts("quit : saves the file system tree, then terminate the program.");
	puts("menu : shows a menu of valid commands."); 
}
//quit : save the file system tree, then terminate the program.
int quit(char *name){
	return save(name);
}


