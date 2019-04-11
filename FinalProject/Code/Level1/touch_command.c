//
// Created by austinmm on 4/9/19.
//
#include "Level1.h"

int _touch(char* pathname)
{
    int r = _creat(pathname);
    //if the file already exist
    if(r == -2){
        //update its access time to now
        int ino = getino(pathname);
        MINODE* mip = iget(_Running->cwd->dev, ino);
        time_t now;
        //Access - the last time the file was read
        mip->INODE.i_atime = time(&now);
        printf("The Access Time of '%s' has been Updated to Current Time.", basename(pathname));
        return 0;
    }
    return r;
}