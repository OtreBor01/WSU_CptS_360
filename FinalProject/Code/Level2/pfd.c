//
// Created by austinmm on 4/18/19.
//

#include "Level2.h"

char* flag_to_string(int flag){
    switch(flag){
        case READ: return "READ";
        case WRITE: return "WRITE";
        case RDWR: return "RD/WR";
        case APPEND: return "APPEND";
        default: return "N/A";
    }
}

int _pfd(char*pathname)
{
    printf("\n|=====================   pid = %d   =====================\n", _Running->pid);
    printf("| fd    mode    count   offset   [dev,ino]   filename\n");
    printf("| --   ------   -----   ------   ---------   --------\n");
    OFT* oft;
    for(int i = 0; i < NUM_FD; i++){
        oft = _Running->fd[i];
        if(oft != NULL && oft->minodePtr != NULL)
        {
            char* flag = flag_to_string(oft->mode);
            printf("| %02d   %6s   %5d   %6d   [%02d, %02d]    %8s\n",
                    i, flag, oft->refCount, oft->offset, oft->minodePtr->dev, oft->minodePtr->ino, oft->fileName);
        }
    }
    printf("|=======================================================\n");
    return 0;
}