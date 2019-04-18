//
// Created by noah on 4/18/19.
//
#include "Level2.h"
int _close(char* fdString){

    int fd = atoi(fdString); //Convert fd to int
    if (fd > NUM_FD){ //if file descriptor is not in range reject
        print_notice("File descriptor out of range.");
        return -1;
    }

    //Starting to check
    if(_Running->fd[fd]!=NULL){ //If file descriptor in process is not null
        _Ofts[fd].refCount--; //decrease refcount of OFT
        if(_Ofts[fd].refCount == 0){  //If last process using OFT
            iput(_Ofts[fd].minodePtr); //Release minode
        }
    }
    clearOftEntry(fd); //Clear OFT entry
    _Running->fd[fd] = 0; //Clear Process entry
    printf("|Close file descriptor: %d",fd);

}