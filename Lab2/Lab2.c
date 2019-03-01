//A relative path is a path that describes the location of a file or folder in relative to the current working directory.
//An absolute path is a path that describes the location of a file or folder regardless of the current working directory; in fact, it is relative to the root directory.
//Libraries neccesary include 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <libgen.h>
#include <ctype.h>
//Defined Constants
#define PATH 64
#define LINE 128
#define CMD 16
#define ROOT "~"
#define ABSPATH "/"
//Custom Data Structures and Types 
typedef struct node{
	char Name[PATH];
	char Type;
	struct node *Child, *Sibling, *Parent;
}Node;

//Function Declarations
Node *createNode(char*,char);   char *getPath(Node*);
void dbname(char*); 			Node *locateDir(char*);
Node *traverseDir(char*); 		void initialize();
int findCmd(char*); 			int mkdir(char*);
int rmdir(char*);   			int ls(char*);
int cd(char*);      			int pwd(char*);
int creat(char*);   			int rm(char*);
int reload(char*);  			int save(char*);
int menu(char*);    			int quit(char*);
void displayNode(Node*); 		int runCommand(char*,char*);
void saveTree(FILE*,Node*,char*); void resetToRoot(bool isWorking);
//Gloabal Variables
Node *root, *cwd, *start; // root and CWD (Current Working Directory) pointers
char pathname[PATH] = ROOT, cwPath[PATH] = "",  dname[PATH] = "", bname[PATH] = "";
char *cmd[] = {"mkdir", "rmdir", "ls", "cd", "pwd", "creat", "rm",
			   "reload", "save", "menu", "quit", NULL};
int (*fptr[ ])(char *)={(int (*)())mkdir,rmdir,ls,cd,pwd,creat,rm,reload,save,menu,quit};

int main(void){
	initialize(); //initialize root node of the file system tree
	while(true){
		cwd = start;
		strcpy(cwPath, pathname);
		char line[LINE] = "", path[PATH] = "", command[CMD] = "";
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
	if(path[0] == ABSPATH[0]){ 
		resetToRoot(true);
		path++;
	}
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
void resetToRoot(bool isWorking){
	if(isWorking){
		cwd = root; 
		strcpy(cwPath, ROOT);
	}
	else{
		start = root; 
		strcpy(pathname, ROOT);
	}
}
Node * traverseDir(char *name){
	Node *child = cwd->Child;
	while(child){
		if(child->Type == 'D' && !strcmp(child->Name, name)){
			cwd = child;
			return child;
		}
		child = child->Sibling;
	}
	return NULL;
}
Node *locateDir(char *path)
{
	char *delimiter = "/";
	char *token = strtok(path, delimiter); // first call to strtok()
	while(token){
		if(!strcmp(token, "..") && cwd != root){
			cwd = cwd->Parent;
			//Searches for the last occurrence of a character in the string
			char *substr = strrchr(cwPath, '/');
			int n = strlen(cwPath)-strlen(substr);
			cwPath[n] = '\0';
		}
		else if((cwd=traverseDir(token)) == NULL){ //If invalid directory name
			return NULL;
		}
		else if(strlen(token) != 0){
			strcat(cwPath, "/");
			strcat(cwPath, cwd->Name);
		}
		token = strtok(NULL, delimiter); // call strtok() until it returns NULL
	}
	return cwd;
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
	if(strlen(name) == 0){ 
		resetToRoot(false);
		return true;
	}
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
	if(!(node->Parent)){ //root directory
		return ABSPATH; 
	}
	char parentPath[PATH];
	strcpy(parentPath, getPath(node->Parent));// + node->Name;
	strcat(parentPath, node->Name);
	if(node != cwd){ strcat(parentPath, "/"); }
	char *path = (char*) malloc(1 + strlen(parentPath));
	strcpy(path, parentPath);
	return path;
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
	if(fp == NULL){ 
		printf("reload: failed to locate '%s': No such file exist\n", name); 
		return false;
	}
	char line[LINE];
	initialize();
	while(fgets(line, LINE, fp)){
		line[strlen(line)-1] = 0; // kill \n at end of line
		char type[2], path[PATH], *command = "mkdir";
		sscanf(line, "%s %s", type, path); // print a line to file
		if(type[0] == 'F'){ command = "creat"; }
		int r = runCommand(command, path);
		cwd = root;
	}
	fclose(fp); // close FILE stream when done
	return true;
}
//save filename :save the current file system tree as a file
int save(char *name){
	FILE *fp = fopen(name, "w"); // fopen a FILE stream for WRITE
	if(fp == NULL){ 
		printf("save: failed to create '%s': Invalid file name\n", name);
		return false;
	}
	saveTree(fp, root->Child, "");
	fclose(fp); // close FILE stream when done
	return true;
}
void saveTree(FILE *fp, Node *node, char *path){
	if(node){
		char currPath[PATH];
		strcpy(currPath, path);
		strcat(currPath, "/"); strcat(currPath, node->Name);
		fprintf(fp, "%c %s\n", node->Type, currPath); // print a line to file
		if(node->Child){ saveTree(fp, node->Child, currPath); }
		if(node->Sibling){ saveTree(fp, node->Sibling, path); }
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
	if(save("quit.txt")){
			puts("Directory Tree has been saved under the file \"quit.txt\"");
			return true;
	}
	return false;
}
