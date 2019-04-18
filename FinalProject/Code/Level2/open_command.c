//
// Created by noah on 4/17/19.
//
#include "Level2.h"

int truncateHelper(MINODE* mip){
    for(int i = 0; i < 14; i++){ //Releases all minode data blocks including double
                                 //indirect and indirect (12,13)
        int blk = mip->INODE.i_block[i];
        bdalloc(mip->dev,blk);
    }
}


int openCmd(char *filename, int flags){
    //Get files minode
    int ino = getino(filename);

    if(ino == 0){//if file doesnt exist
        _creat(filename); //create file
        ino = getino(filename);
    }
    MINODE* mip = iget(_Running->cwd->dev, ino);

    if(!S_ISREG(mip->INODE.i_mode)){
        print_notice("File type is not regular");
        return -1;
    }

    //Check if file is open in a incompatable mode
    OFT* coftp = &_Ofts[0];
    int c = 0;
    for(; c< NUM_OFT & coftp->minodePtr != NULL; c++){
        if(coftp->minodePtr->ino == mip->ino & coftp->mode != 0){//If file is open and type incompatible
            print_notice("File already open with incompatible type");
            return -1;
        }
        coftp = &_Ofts[c];
    }



    //allocate an openTable entry OFT; initilaize OFT entries
    if(_OpenOFT+1 > NUM_OFT){return -1;}

    OFT* oftp = &_Ofts[_OpenOFT++];
    oftp->mode = flags;
    oftp->refCount =1;
    oftp->minodePtr = mip;

    //Depending on the open mode 0|1|2|3, set the OFT's offset accordingly:
    switch(flags){
        case 0:
            oftp->offset = 0;
            break;
        case 1:
            truncateHelper(oftp->minodePtr);
            oftp->offset = 0;
            break;
        case 2:
            oftp->offset = 0;
            break;
        case 3:
            oftp->offset = mip->INODE.i_size;
            break;
        default:
            print_notice("invalid mode\n");
            return -1;

    }

    //find the SMALLEST i in running PROC's fd[ ] such that fd[i] is NULL
    int i = 0;
    for(; _Running->fd[i] != NULL;i++);
    //      Let running->fd[i] point at the OFT entry
    _Running->fd[i] = oftp;


    //update INODE's time field
    time_t now;
    oftp->minodePtr->INODE.i_atime = time(&now);//update INODE's atime field
    if(flags == 2 || flags == 3 || flags == 1) {//RW APPEND and W
        oftp->minodePtr->INODE.i_mtime = time(&now);//update INODE's mtime field
    }
    oftp->minodePtr->dirty=1;//mark node as dirty
    return i;


}

int _open(char* pathname){
    char filename[100];
    int mode=-1;
    int fd;
    sscanf(pathname,"%s %d", filename, &mode);
    fd =openCmd(filename, mode);
    return fd;
}
