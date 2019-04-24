//
// Created by austinmm on 4/23/19.
//

#ifndef LEVEL3_H
#define LEVEL3_H
/*
-------  LEVEl 3 ------------
mount, umount
File permission checking
-----------------------------
*/

//***** Libraries *****
#include "../Header.h"


//***** Defined Constants *****


//***** Global Variables *****
int _Total_Mounts;

//***** Function Prototypes *****
//*** Commands ***
int _mount(char*);
int _unmount(char*);

//*** Utility Functions ***
int mountStat(void);
int checkMounted(char*);
int checkCwdBusy(char*);
int mount_root(char*,char*);
int get_mount(char*);
int checkBusyFiles(char*);
#endif //LEVEL3_H
