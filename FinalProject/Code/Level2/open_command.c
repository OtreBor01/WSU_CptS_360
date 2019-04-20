//
// Created by noah on 4/17/19.
//
#include "Level2.h"

int _open(char* pathname){
    char filename[PATH_SIZE];
    int mode=-1;
    int fd;
    sscanf(pathname,"%s %d", filename, &mode);
    fd = open_file(filename, mode);
    return fd;
}
