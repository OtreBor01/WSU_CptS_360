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
    // byte offset in file to READ
    OFT* oft = _Running->fd[fd];
    int offset = oft->offset;
    MINODE* mip = oft->minodePtr;
    //compute bytes available in file: avil = fileSize – offset;
    int fileSize = mip->INODE.i_size;
    int bytes_available = fileSize - offset;
    char kbuf[BLKSIZE];
    int count = 0;
    while (nbytes && bytes_available)
    {
        memset(kbuf, 0, BLKSIZE);
        //compute logical block:
        int lbk = offset / BLKSIZE;
        //start byte in block:
        int start = offset % BLKSIZE;
        //convert logical block number, lbk, to physical block number, blk, through INODE.i_block[ ] array;
        int blk = lbk_to_blk(mip, lbk);
        // read blk into char kbuf[BLKSIZE];
        get_block(mip->dev, blk, kbuf);
        char *k_cp = kbuf + start;
        char buf[BLKSIZE];
        char *cp = buf;
        //the number of bytes remaining in the logical block
        int remain = BLKSIZE - start;
        while (remain){
            // copy bytes from kbuf[ ] to buf[ ]
            int kbuf_size = sizeof(k_cp)*sizeof(char);
            memcpy(cp, k_cp, kbuf_size);
            //increment buf and kbuf
            cp++; k_cp++;
            // inc offset, count;
            offset++; count++;
            // dec remain, avail, nbytes;
            remain--; bytes_available--; nbytes--;
            if (nbytes == 0 || bytes_available == 0) {
                break;
            }
        }
    }
    return count;
}