//
// Created by noah on 4/22/19.
//

//
// Created by noah on 4/18/19.
//

#include "Level2.h"

int _lseek(char* arguments){
    int fd, position, originalPosition;
    sscanf(arguments,"%d %d",&fd, &position);
    OFT *oftp = &_Ofts[fd];
    if(oftp == 0){
        print_notice("File discriptor is null");
        return -1;
    }
    if(position < 0 || position > oftp->minodePtr->INODE.i_size){
        print_notice("Position for offset out of range\n");
        return -1;
    }

    originalPosition = oftp->offset;
    oftp->offset = position;
    printf("|Successfully moved File descriptor[%d] offset to %d",fd,position);
    return originalPosition;

}