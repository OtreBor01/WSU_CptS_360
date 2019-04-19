//
// Created by austinmm on 4/17/19.
//

#include "Level2.h"


/*
    Reads nbytes from an opened file descriptor into a buffer area in user space. The read system call is
    routed to the read function in the OS kernel.
    Preparations:
        ASSUME: file is opened for RD or RW;
        ask for a fd  and  nbytes to read;
        verify that fd is indeed opened for RD or RW;
        return(myread(fd, buf, nbytes));
 */
int _read(char* pathname)
{
    // number of bytes read
    int nbytes = 0, fd = 0;
    sscanf(pathname, "%d %d", &fd, &nbytes);
    char buf[BLKSIZE];
    printf("******************** Read (fd = %02d, nbytes = %d)  ********************\n", fd, nbytes);
    nbytes = read_file(fd, buf, nbytes);
    puts(buf);
    puts("\n*********************************************************************");
    return nbytes;
}