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

//***** Global Variables *****

//***** Function Prototypes *****
int _mv(char*);
int _open(char *);

#endif