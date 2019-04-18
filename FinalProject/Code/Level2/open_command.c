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
    time_t now;
    mip->INODE.i_atime=time(&now);//update times
    mip->INODE.i_mtime=time(&now);
    mip->INODE.i_size = 0;
    mip->dirty=1;
}


int openCmd(char *filename, int flags){
    //Get files minode
    int ino = getino(filename);
    if(ino == 0){//if file doesnt exist
        ino = _creat(filename); //create file
    }

    MINODE* mip = iget(_Running->cwd->dev, ino);
     //check if file is regular
    if(!S_ISREG(mip->INODE.i_mode)){
        print_notice("open: file type is not regular");
        return -1;
    }

    //Check if file is open in a compatible mode
    OFT* oft = &_Ofts[0];
    int c = 0;
    for(; (c < NUM_OFT) && (oft->minodePtr != NULL) ; c++)
    {
        //If file is already open and has an incompatible type
        if(oft->minodePtr->ino == mip->ino & oft->mode != 0)
        {
            print_notice("open: file already open with incompatible type");
            return -1;
        }
        oft = &_Ofts[c];
    }

    //allocate an openTable entry OFT; initilaize OFT entries
    if(_OpenOFT >= NUM_OFT){
        print_notice("open: unable to allocate anymore space to open new file");
        return -1;
    }

    oft = &_Ofts[_OpenOFT++];
    //Depending on the open mode 0|1|2|3, set the OFT's offset accordingly:
    switch(flags){
        case READ: //0
            oft->offset = 0;
            break;
        case WRITE: //1
            truncateHelper(mip);
            oft->offset = 0;
            break;
        case RDWR: //2
            oft->offset = 0;
            break;
        case APPEND: //3
            oft->offset = mip->INODE.i_size;
            break;
        default:
            print_notice("open: invalid mode entered\n");
            return -1;
    }
    oft->mode = flags;
    oft->refCount =1;
    oft->minodePtr = mip;

    //update INODE's time field
    time_t now;
    oft->minodePtr->INODE.i_atime = time(&now);//update INODE's atime field
    if(flags == 2 || flags == 3 || flags == 1) {//RW APPEND and W
        oft->minodePtr->INODE.i_mtime = time(&now);//update INODE's mtime field
    }
    oft->minodePtr->dirty = 1;//mark node as dirty
    strcpy(oft->fileName, filename);

    //find the SMALLEST i in running PROC's fd[ ] such that fd[i] is NULL
    int i = 0;
    for(; _Running->fd[i] != NULL; i++);
    //Let running->fd[i] point at the OFT entry
    _Running->fd[i] = oft;

    return i;
}

int _open(char* pathname){
    char filename[100];
    int mode=-1;
    int fd;
    sscanf(pathname,"%s %d", filename, &mode);
    fd = openCmd(filename, mode);
    return fd;
}
