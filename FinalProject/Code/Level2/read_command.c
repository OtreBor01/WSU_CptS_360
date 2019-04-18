//
// Created by austinmm on 4/17/19.
//

#include "Level2.h"

int myread(int fd, char buf[ ], int nbytes)
{
    return 0;
}
/* Algorithm of Converting Logical Blocks to Physical Blocks */
int lbk_to_blk(MINODE* mip, int lbk)
{
    int ibuf[256];
    // convert lbk to blk via INODE
    if (lbk < 12)// direct blocks
    {
        return mip->INODE.i_block[lbk];
    }
    else if (12 <= lbk < 12+256)// indirect blocks
    {
        int blk = mip->INODE.i_block[12];
        get_block(mip->dev, blk,ibuf);
        return ibuf[lbk-12];
    }
    else// doube indirect blocks; see Exercise 11.13 below.
    {
        //this is an educated quess!!!
        int i = 13, blk = mip->INODE.i_block[12];
        for(; i <= 15; i++){
            if(i <= lbk < i+256){
                break;
            }
        }
        int block = (i - 1) / 8 + _IStartBlock;
        int offset = (i - 1) % 8;
        get_block(mip->dev, block, ibuf);
        return ibuf + offset;
    }
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
    OFT* oft = &_Ofts[_OpenOFT]; //_Running->fd[0]
    int offset = oft->offset;
    MINODE* mip = oft->minodePtr;
    //compute bytes available in file: avil = fileSize – offset;
    int fileSize = mip->INODE.i_size;
    int bytes_available = fileSize - offset;
    char kbuf[BLKSIZE];
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
        int count = 0;
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
    return 0;
}