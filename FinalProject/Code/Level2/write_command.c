//
// Created by austinmm on 4/18/19.
//

#include "Level2.h"

/*
 Writes nbytes from buf in user space to an opened file descriptor and returns the actual number of bytes written.
 The write system call is routed to the write function in the OS kernel.
 */
int _write(char* pathname)
{
    // number of bytes read
    char buf[BLKSIZE];
    int nbytes = 0, fd = 0;
    sscanf(pathname, "%d %[^\n]", &fd, buf);
    nbytes = strlen(buf);
    nbytes = write_file(fd, buf, nbytes);
}