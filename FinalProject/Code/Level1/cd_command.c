//
// Created by austinmm on 3/28/19.
//
#include "Level1.h"

int _cd(char* path)
{
    if(!strcmp(path, "")) //not a valid path
    {
        return 0;
    }
    else{
        char buf[BLKSIZE];
        //search for path to print
        int ino = getino(path); //get inode number of the dir to cd into
        if(ino == 0) { return -1; } //if unable to find path to dir specified
        MINODE *mip = iget(_Running->cwd->dev, ino); //Using the inode number retrieved we find the actual MINODE object
        if(S_ISDIR(mip->INODE.i_mode)) //if the inode is a valid directory
        {
            if(_Running->cwd != mip){
                iput(_Running->cwd);
            }
            _Running->cwd = mip;
        }
        else if(S_ISREG(mip->INODE.i_mode)) //if inode was a file
        {
            printf("Cannot cd into a file\n");
        }
    }
    return 0;
}
