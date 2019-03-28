#ifndef HEADER_H
#define HEADER_H

//***** Libraries *****
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <libgen.h>
#include <sys/stat.h>
#include <ext2fs/ext2_fs.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include "Type.h"

//***** Global Variables *****
MINODE minode[NMINODE];
MINODE *root;
PROC   proc[NPROC], *running;

int _DEV;
int inode_start;


//***** Function Prototypes *****
//Commands.c Functions
int ChangeDir(int,char*), ListFile(int,char*), Pwd(int,MINODE*), Quit(void);
int R_Pwd(char*,int,MINODE*);
char* GetName(INODE*,int);

//until.c Functions
int get_block(int,int,char*);
int put_block(int,int,char*);
int tokenize(char*, char**);
MINODE *iget(int,int);
void iput(MINODE*);
int search(MINODE*,char*);
int getino(MINODE*, char*);

//Helper.c Functions
void Pause(void);

//***** ls_l.c Functions *****
int ls_reg(MINODE*,char*), ls_dir(MINODE*);
void mode_to_letters(mode_t, char*);

#endif
