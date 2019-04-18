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
    int nbytes = 0, fd = 0;
    sscanf(pathname, "%d %d", &fd, &nbytes);
    // byte offset in file to READ
    OFT* oft = _Running->fd[fd];
    int offset = oft->offset;
    MINODE* mip = oft->minodePtr;
    //compute bytes available in file: avil = fileSize – offset;
    int fileSize = mip->INODE.i_size;
    char kbuf[BLKSIZE];
    int count = 0;
    while (nbytes)
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
            int buf_size = sizeof(cp)*sizeof(char);
            memcpy(k_cp, cp, buf_size);
            //increment buf and kbuf
            cp++; k_cp++;
            // inc offset, count;
            offset++; count++;
            // dec remain, nbytes;
            remain--; nbytes--;
            if (offset > fileSize) { fileSize++; } // inc file size
            if (nbytes == 0) { break; }
        }
        put_block(mip->dev, blk, kbuf);
    }
    // mark minode dirty for iput() when fd is closed
    mip->dirty = 1;
    return count;
}