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
    char buf[BLKSIZE] = "";
    int nbytes = 0, fd = 0;
    sscanf(pathname, "%d %s", &fd, buf);
    nbytes = strlen(buf);
    // byte offset in file to READ
    OFT* oft = get_open_fd(fd);
    if(oft == NULL){
        print_notice("write: fd has not been open yet");
        return -1;
    }
    if(!fd_can_write(oft->mode)){
        print_notice("write: fd is not open to read");
        return -1;
    }
    int offset = oft->offset;
    MINODE* mip = oft->minodePtr;
    //compute bytes available in file: avil = fileSize – offset;
    char kbuf[BLKSIZE];
    int count = 0;
    while (nbytes > 0)
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
        char *cp = buf;
        //the number of bytes remaining in the logical block
        int remain = BLKSIZE - start;
        while (remain > 0) // write as much as remain allows
        {
            // copy bytes from buf[ ] to kbuf[ ]
            *k_cp++ = *cp++;
            // inc offset, count;
            offset++; count++;
            // dec remain, nbytes;
            remain--; nbytes--;
            // especially for RW|APPEND mode
            if (offset > mip->INODE.i_size) { mip->INODE.i_size++; } // inc file size
            if (nbytes <= 0) { break; }
        }
        put_block(mip->dev, blk, kbuf);
    }
    // mark minode dirty for iput() when fd is closed
    mip->dirty = 1;
    printf("write: wrote %d char into file descriptor fd = %d\n", count, fd);
    return count;
}