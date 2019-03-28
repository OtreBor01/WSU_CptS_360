//Page 395 (13.17.2 Helps and Hints)
//***** Libraries *****
#include "ls_l.c"


//***** Function Prototypes *****
int _ls(char*), _cd(char*), _pwd(char*), _mkdir(char*), _rmdir(char*), _rm(char*), _cat(char*);
int getLineInfo(char*,char*,char*);


//***** Global Variables *****
char *cmd[] = {"ls","cd","pwd","mkdir","rmdir","rm","cat",NULL};
int (*fcmds[])(char *)={(int (*)())_ls,_cd,_pwd,_mkdir,_rmdir,_rm,_cat};


//***** Function Declaration *****
//mkdir: make a directory with pathname
int _mkdir(char* pathname){
	// https://linux.die.net/man/3/mkdir
	return mkdir(pathname, 0755);
}

//rmdir: remove directory named pathname
int _rmdir(char* pathname){
	// https://linux.die.net/man/3/rmdir
	return rmdir(pathname);
}

//rm: remove file named pathname
int _rm(char* pathname){
	// http://man7.org/linux/man-pages/man2/unlink.2.html
	return unlink(pathname);
}

//cd: change Current Working Directory (CWD) to pathname
int _cd(char* pathname){
	// http://man7.org/linux/man-pages/man2/chdir.2.html
	return chdir(pathname);
}

//pwd: show absolute pathname of CWD
int _pwd(char* pathname){
	char buf[MAX];
	// http://man7.org/linux/man-pages/man3/getcwd.3.html
	char *cwd = getcwd(buf, MAX);
	printf("pwd: '%s'\n", cwd);
	return (cwd != NULL)? (int)cwd : -1;
}

//ls: list CWD or pathname in the same format as ls –l of Linux
int _ls(char* pathname){
	// http://man7.org/linux/man-pages/man2/fstat.2.html
	struct stat mystat, *sp = &mystat;
	char path[1024], cwd[256];
	if(pathname == NULL || !strlen(pathname)){
		strcpy(path,  "./"); // default to CWD
	}
	else{
		strcpy(path, pathname);
	}
	printf("Pathname: '%s'\n", path);
	if (lstat(path, sp) < 0){
		printf("Error: No Such Path '%s'\n", path);
		exit(1);
	}
	if (path[0] != '/'){ // pathname is relative : get CWD path
		getcwd(cwd, 256);
		strcat(cwd, "/"); strcat(cwd, path);
	}
	int r;
	if (S_ISDIR(sp->st_mode)){ r = ls_dir(path); }
	else { r = ls_file(path); } 
	if(!isServer){	
		char *str = (char*)r;
		puts(str);
		free(str);
		r = 0;
	}
	return r;
}


int _cat(char* pathname){
	char buff[MAX];
	int n, fd;
	fd = open(pathname, O_RDONLY);
	if (fd == -1)
	{
	  printf("\tError: %s File Not Found\n", pathname);
	  return (-1);
	}
	while ((n = read(fd, buff, MAX)) > 0){
		write(1, &buff, n);
	}
	close(fd);
	return 0;
}

int getLineInfo(char *line, char *command, char *pathname){
	char *path, tempLine[MAX]; strcpy(tempLine, line); 
	if((path = strchr(tempLine, ' ')) != NULL){
		path++; //removes space from pathname
		strcpy(pathname, path);
		strcpy(command,strtok(tempLine, " "));
	}
	else{ 
		strcpy(pathname, "");
		strcpy(command, line);
	}
	for(int i = 0; cmd[i] != NULL ; i++){
		if(!strcmp(cmd[i], command)){ 
			printf("Command: %s [Index = %d] || Path: %s\n", 
				command, i, (pathname == NULL)? "NULL": pathname);
			return i; 
		}
	}
	return -1;
}


