//
// Created by austinmm on 4/9/19.
//
#include "Level1.h"

//chmod oct filename: Change filename’s permission bits to octal value
int _chmod(char* pathname)
{
    char permission[10] = "";
    sscanf(pathname, "%s %s", permission, pathname);
    __u16 octal = strtol(permission, NULL, 8);
    //int octal = convertDecimalToOctal(permission);
    if(permission == 0)
    {
        print_notice("Invalid permission value entered");
    }
    //(1). get the in-memory INODE of a file by
    int ino = getino(pathname);
    if(ino == 0)
    {
        print_notice("Unable to locate the file specified");
        return -1;
    }
    MINODE* mip = iget(_Running->cwd->dev, ino);
    //(2). get information from INODE or modify the INODE;
    //int octal = permission & (S_IRWXU | S_IRWXG | S_IRWXO);
    mip->INODE.i_mode = octal;
    //(3). if INODE is modified, set mip->dirty to zonzero for write back;
    mip->dirty = 1;
    iput(mip);
    return 0;
}