//
// Created by noah on 4/7/19.
//

#include "Level1.h"

int _readlink(char* pathname){

    int dev = _Running->cwd->dev;
    int ino = getino(pathname, &dev);
    MINODE* mip = iget(dev,ino);
    INODE* ip = &mip->INODE;
    int lino;
    MINODE * lmip;
    char buf[BLKSIZE];
    char *cp;
    char linkPath[BLKSIZE];

    if(!S_ISLNK(ip->i_mode)) { //Check if file is a link
        print_notice("File type is not LINK");
        return -1;
    }
    int i = 0;
    cp = ip->i_block[i];
    while(cp && cp < BLKSIZE){ // grab path name from inside the iblocks
        linkPath[i] = cp;
        i++;
        cp = ip->i_block[i];
    }
    //grab ino from pathname found in link file iblocks
    lino = getino(linkPath, &dev);
    //grab mip for file linked to
    lmip = iget(dev, lino);
    printf("Link file size: %d",lmip->INODE.i_size);
    return lmip->INODE.i_size;




    /*
    int blk = ip->i_block[0]; //gets the size of the first block index of the inode ip
    get_block(mip->dev, blk, buf); //reads in that blocks data into buf
    DIR* dp = (DIR*)buf;
    dp->name[dp->rec_len] = 0;
    printf("\nName Length: %s\n",strlen(dp->name));
    return strlen(dp->name);
    */






}
