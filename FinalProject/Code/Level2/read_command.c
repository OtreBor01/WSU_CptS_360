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

    /*(1). count = 0;
    // number of bytes read
    offset = OFT.offset;
    // byte offset in file to READ
    compute bytes available in file: avil = fileSize – offset;
    (2). while (nbytes && avil){
        compute logical block: lbk
                                       = offset / BLKSIZE;
        start byte in block:
        start = offset % BLKSIZE;
        (3).
                convert logical block number, lbk, to physical block number, blk,
        through INODE.i_block[ ] array;
    */
}