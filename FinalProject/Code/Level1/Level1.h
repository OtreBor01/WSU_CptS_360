#ifndef LEVEL1_H
#define LEVEL1_H
/*
-------  LEVEL 1 Requirements ------------
mount_root;
mkdir, rmdir, ls, cd, pwd;
creat, link,  unlink, symlink
stat,  chmod, touch;

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
int _cd(char*);
int _pwd(MINODE*);
int _ls(char*);
int _link(char*);
int _creat(char*);
int _touch(char*);
int _rm(char*);
int _mkdir(char*);
int _rmdir(char*);
int _symlink(char*);
int _readlink(char*);
int _unlink(char*);
int _stat(char*);
int _chmod(char*);
void mode_to_letters(mode_t,char*);

#endif