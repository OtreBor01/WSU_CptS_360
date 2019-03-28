/*
The following shows a simple ls program which behaves like the ls –l command of Linux. The purpose
here is not to re-invent the wheel by writing yet another ls program. Rather, it is intended to show how
to use the various syscalls to display information of files under a directory. By studying the example
program code, the reader should also be able to figure out how Linux’s ls command is implemented.
*/

//***** Libraries *****
#include "header.h"

//***** Function Prototypes *****
int ls_file(char*), ls_dir(char*);
void mode_to_letters(mode_t, char*);

//***** Global Variables *****

void mode_to_letters(mode_t mode, char *str)
{
	//dir
	if (S_ISDIR(mode)) { str[0] = 'd'; }
	//link
	else if (S_ISLNK(mode)) { str[0] = 'l'; }
	//reg, i.e. if (S_ISREG(mode))
	else { str[0] = '-'; }
    str[1] = (mode & S_IRUSR) ? 'r' : '-';
    str[2] = (mode & S_IWUSR) ? 'w' : '-';
    str[3] = (mode & S_IXUSR) ? 'x' : '-';
    str[4] = (mode & S_IRGRP) ? 'r' : '-';
    str[5] = (mode & S_IWGRP) ? 'w' : '-';
    str[6] = (mode & S_IXGRP) ? 'x' : '-';
    str[7] = (mode & S_IROTH) ? 'r' : '-';
    str[8] = (mode & S_IWOTH) ? 'w' : '-';
    str[9] = (mode & S_IXOTH) ? 'x' : '-';
    str[10] = '\0';
}

//***** Function Declaration *****
int ls_file(char *fname)
{
	struct stat fstat, *sp;
	char ftime[64], *file_ls = (char*) malloc(sizeof(char*)*250);
	strcpy(file_ls, "");
	sp = &fstat;
	if (lstat(fname, sp) < 0){
		printf("Notice: Unable to stat '%s'\n", fname);
		return (int)NULL;
	}
	char mode[11] = "";
	memset(mode,0,sizeof(mode));
	mode_to_letters(sp->st_mode, mode);
	strcat(file_ls, mode);
	char *file_info = (char*) calloc(50, sizeof(char*));
	memset(file_info,0,sizeof(file_info));
	sprintf(file_info, "%3d %3d %3d %8ld ", sp->st_nlink, sp->st_gid, sp->st_uid, sp->st_size);
	strcat(file_ls, file_info);
	free(file_info);
	strcpy(ftime, ctime(&sp->st_ctime)); //file time created in calendar form
	ftime[strlen(ftime)-1] = 0; // kill \n at end
	strcat(file_ls, ftime); strcat(file_ls, " "); //file time created
	strcat(file_ls, basename(fname)); //file basename 
	//add '/' if directory to indicate it in its name
	if(S_ISDIR(sp->st_mode)){ strcat(file_ls,"/"); }
	// get linkname if is a symbolic file
	if (S_ISLNK(sp->st_mode)){
		char linkname[50];
		// ssize_t readlink(const char *path, char *buf, size_t bufsiz): reads linkname
		readlink(fname, linkname, sizeof(linkname));
		strcat(file_ls, " -> ");
		strcat(file_ls, linkname); //linked name
	}
	strcat(file_ls, "\n");
	return (int)file_ls;
}

int ls_dir(char *dname)
{
	if(dname == NULL || !strlen(dname)){
		return (int)NULL;
	}
	char files_ls[5000];
	memset(files_ls,0,sizeof(files_ls)); 	
	int len;
    struct dirent *pDirent;
    DIR *pDir;
    pDir = opendir(dname);
    if (pDir == NULL) {
        printf ("Error: Failure to Open Directory '%s'\n", dname);
        return 1;
    }
    for(int i = 0; (pDirent = readdir(pDir)) != NULL; i++) {
    	if(isServer){
    		printf("ls_file %s%s\n", dname, pDirent->d_name);
    	}
        char *file = (char*) ls_file(pDirent->d_name);
        if(file != NULL){
			strcat(files_ls, file);
		}
		free(file);
    }
    closedir(pDir);
    char *allFiles = (char*)malloc(sizeof(char*)*(strlen(files_ls)+1));
    strcpy(allFiles, files_ls);
	memset(files_ls,0,sizeof(files_ls)); 	
    return (int)allFiles;
}

