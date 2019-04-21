//
// Created by noah on 4/18/19.
//
#include "Level2.h"

int _close(char* fdString){

    int fd = atoi(fdString); //Convert fd to int
    if (fd > NUM_FD || fd < 0){ //if file descriptor is not in range reject
        print_notice("File descriptor out of range.");
        return -1;
    }
    close_file(fd);
}