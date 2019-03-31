#ifndef HEADER_H
#define HEADER_H

//***** Libraries *****
#include <stdio.h> //printf, sscanf, getchar, puts
#include <stdlib.h> //malloc, calloc, free, exit
#include <string.h> //strcpy, strcat, memset, strcmp, strlen, strtok
#include <time.h> //ctime
#include <fcntl.h>
#include <libgen.h>
#include <sys/stat.h> //S_ISREG, S_ISDIR
#include <ext2fs/ext2_fs.h> //inode stuff
#include <unistd.h>

#include "Type.h"

//***** Defined Constants *****
#define PATH_DELIMITER      "/"
#define PATH_SIZE           256
#define PATH_TOKENS         20



//***** Global Variables *****
char    _Path[PATH_SIZE];
char*   _PathTokens[PATH_TOKENS];
int     _PathTokenCount;


//***** Function Prototypes *****
//Utility_functions: Function Prototypes
int tokenize(char*);
int search(MINODE*,char*);
void print_error(char*,char*);
MINODE* mialloc(void);
int midalloc(MINODE*);
int get_block(int,int,char*), put_block(int,int,char*);
MINODE *iget(int,int);
int iput(MINODE*);
int getino(char*);
int enter_name( MINODE *pip, int ino, char *name);
int balloc(int dev);
int ialloc(int dev);
int set_bit(char *buf, int bit);
int clr_bit(char *buf, int bit);
int tst_bit(char *buf, int bit);


//Level1: Function Prototypes
extern int _cd(char*);
extern int _pwd(MINODE*);
extern int _ls(char*);

#endif

//#include <fcntl.h>
//#include <libgen.h> //basename, dirname
//#include <sys/types.h> //http://pubs.opengroup.org/onlinepubs/009696699/basedefs/sys/types.h.html
//#include <unistd.h> //pipe, fork, execve, read, write