#ifndef LEVEL2_H
#define LEVEL2_H
/*
 -------  LEVEl 2 -------------
 open,  close,  read,  write
 lseek  cat,    cp,    mv

step0  : mount disk as the root FS; ls, pwd, cd, quit
step1  : mount disk as the root FS; ls,pwd,cd, mkdir, creat
step2  : ls,pwd,cd; mkdir,creat;  rmdir
step3  : ls,pwd,cd,mkdir,creat,rmdir;  link, symlink, unlink
*/

//***** Libraries *****
#include "../Header.h"


//***** Defined Constants *****
#define READ 0
#define WRITE 1
#define RDWR 2
#define APPEND 3



//***** Global Variables *****

//***** Function Prototypes *****
//*** Commands ***
int _mv(char*);
int _open(char*);
int _read(char*);
int _pfd(char*);
int _close(char*);
int _write(char*);

//*** Utility Functions ***
char* flag_to_string(int);
int lbk_to_blk(MINODE*, int);
int fd_can_read(int);
int fd_can_write(int);
OFT* get_open_fd(int);


#endif