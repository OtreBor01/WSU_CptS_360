//
// Created by austinmm on 4/18/19.
//
#include "Level2.h"
//converts a open type flag to string
char* flag_to_string(int flag){
    switch(flag){
        case READ: return "READ";
        case WRITE: return "WRITE";
        case RDWR: return "RD/WR";
        case APPEND: return "APPEND";
        default: return "N/A";
    }
}

/* Algorithm of Converting Logical Blocks to Physical Blocks */
int lbk_to_blk(MINODE* mip, int lbk)
{
    int ibuf[256];
    char buf[BLKSIZE];
    // convert lbk to blk via INODE
    if (lbk < 12)// direct blocks
    {
        return mip->INODE.i_block[lbk];
    }
    else if (12 <= lbk < 12+256)// indirect blocks
    {
        int blk = mip->INODE.i_block[12];
        if (blk == 0){
            //allocate a disk block;
            balloc(mip->dev);
            //record it in i_block[12];
            put_block(mip->dev, blk, buf);
        }
        //get i_block[12] into an int ibuf[256];
        get_block(mip->dev, blk,ibuf);
        blk = ibuf[lbk-12];
        return blk;
    }
    else// double indirect blocks; see Exercise 11.13 below.
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

int fd_can_read(int flag)
{
    if(flag == RDWR || flag == READ){
        return 1;
    }
    return 0;
}

int fd_can_write(int flag)
{
    if(flag == WRITE || flag == APPEND || flag == RDWR){
        return 1;
    }
    return 0;
}

OFT* get_open_fd(int fd)
{
    OFT* oft = _Running->fd[fd];
    if(oft == NULL || oft->minodePtr == NULL) {
        return NULL;
    }
    return oft;
}