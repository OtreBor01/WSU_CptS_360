//
// Created by austinmm on 4/17/19.
//

#include "Level2.h"

int myread(int fd, char buf[ ], int nbytes)
{
    return 0;
}
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
    int nbytes = 0;
    sscanf(pathname, "%d %s", &nbytes, pathname);
    // byte offset in file to READ
    int offset = _Ofts[_OpenOFT].offset;
    MINODE* mip = _Ofts[_OpenOFT].minodePtr;
    //compute bytes available in file: avil = fileSize – offset;
    int fileSize = mip->INODE.i_size;
    int bytes_available = fileSize - offset;
    while (nbytes && bytes_available)
    {
        //compute logical block:
        int lbk = offset / BLKSIZE;
        //start byte in block:
        int start = offset % BLKSIZE;
        //convert logical block number, lbk, to physical block number, blk, through INODE.i_block[ ] array;
        break;
    }
    return 0;
}