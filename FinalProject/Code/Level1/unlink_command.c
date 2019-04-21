//
// Created by noah on 4/8/19.
//
#include "Level1.h"

int _unlink(char* pathname){
    int dev = _Root->dev;
    int ino = getino(pathname);
    char parent[500], child[500];
    MINODE* mip = iget(dev, ino);
    //Make sure file type is not a directory
    if(!S_ISLNK(mip->INODE.i_mode)){
        print_notice("File type must be link");
        return -1;
    }
    //remove name entry from parent DIR data block
    strcpy(parent, dirname(pathname));
    strcpy(child, basename(pathname));
    int pino = getino(parent);
    MINODE* pmip = iget(dev, pino);
    int isDir = 0; //ensures that dir with the same name as child doesn't get removed
    remove_name(pmip,child, isDir);
    pmip->dirty=1;
    iput(pmip);
    //Decrement INODE's link count by 1
    mip->INODE.i_links_count--;
    if(mip->INODE.i_links_count>0){
        mip->dirty=1; //for write INODE back to disk
    }
    else{
        //deallocate all iblocks
        for(int i = 0; i < 12 && i < mip->INODE.i_blocks; i++){
            bdalloc(dev,mip->INODE.i_block[i]);
        }

        idalloc(dev, mip->ino);

    }
    iput(mip);
    printf("Successfully unlinked %s\n",pathname);


}
