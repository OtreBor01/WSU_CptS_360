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
//*** Commands ***
int _cd(char*);
int _pwd(char*);
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

//*** Utility Functions ***
void mode_to_letters(mode_t,char*);
int isEmpty(MINODE*);
char* get_inode_name(MINODE*,int);
int kmkdir(MINODE*,char*);
int create_REG_INODE(int,MINODE*);
void mode_to_letters(mode_t,char*);
int check_file_permissions(MINODE* mip);

#endif