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

int replace_file(MINODE* src_mip, MINODE* p_dest_mip, MINODE* dest_mip, char* name)
{
    //if there exist a file to be replaced in the destination then...
    // remove the existing file from the destination parent directory
    int fileType = mode_to_filetype(dest_mip->INODE.i_mode);
    if(dest_mip != NULL) {
        int isDir = (fileType == 2) ? 1 : 0;
        remove_name(p_dest_mip, name, isDir);
    }
    fileType = mode_to_filetype(src_mip->INODE.i_mode);
    enter_name(p_dest_mip, name, src_mip->ino, fileType);
    return -1;
}

//The original file is a directory so it can only be moved into another directory with its original name or...
// under a new one specified by the new path
int mv_into_dir(MINODE* src_mip, MINODE* p_dest_mip, MINODE* dest_mip, char* name)
{
    int fileType = mode_to_filetype(src_mip->INODE.i_mode);
    int result = enter_name(dest_mip, name, src_mip->ino, fileType);
    //-1 would indicate that a file of the same type and name already exist in the current directory so we need to replace it
    if(result == -1){
        return replace_file(src_mip, p_dest_mip, dest_mip, name);
    }
    return result;
}


int seperate_path(char* path, char* source, char* dest)
{
    sscanf(path, "%s %s", source, dest);
    if(!strcmp(source, "")) {
        print_notice("mv: missing file operand");
        return -1;
    }
    else if (!strcmp(dest, "")) {
        print_notice("mv: missing destination file operand");
        return -1;
    }
    return 0;
}


int read_file(int fd, char* buf, int nbytes)
{
    OFT* oft = get_open_fd(fd);
    if(oft == NULL){
        print_notice("read: fd has not been open yet");
        return -1;
    }
    if(!fd_can_read(oft->mode)){
        print_notice("read: fd is not open to read");
        return -1;
    }
    // byte offset in file to READ
    int offset = oft->offset;
    MINODE* mip = oft->minodePtr;
    //compute bytes available in file: avil = fileSize – offset;
    int fileSize = mip->INODE.i_size;
    int bytes_available = fileSize - offset;
    char *cp = buf;
    char kbuf[BLKSIZE];
    int count = 0;
    while (nbytes > 0 && bytes_available > 0)
    {
        memset(kbuf, 0, BLKSIZE);
        memset(buf, 0, BLKSIZE);
        //compute logical block:
        int lbk = offset / BLKSIZE;
        //start byte in block:
        int start = offset % BLKSIZE;
        //convert logical block number, lbk, to physical block number, blk, through INODE.i_block[ ] array;
        int blk = lbk_to_blk(mip, lbk);
        // read blk into char kbuf[BLKSIZE];
        get_block(mip->dev, blk, kbuf);
        char *k_cp = kbuf + start;
        //the number of bytes remaining in the logical block
        int remain = BLKSIZE - start;
        while (remain > 0){
            // copy bytes from kbuf[ ] to buf[ ]
            *cp++ = *k_cp++;
            oft->offset++; // advance offset
            count++; //incr count
            // dec remain, avail, nbytes;
            remain--; bytes_available--; nbytes--;
            if (nbytes == 0 || bytes_available == 0) { break; }
        }
    }
    return count;
}

int write_file(int fd, char buf[], int nbytes)
{
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
        *k_cp = 0; //ensures to overwrite exiting buf
        put_block(mip->dev, blk, kbuf);
    }
    // mark minode dirty for iput() when fd is closed
    mip->dirty = 1;
    printf("write: wrote %d char into file descriptor fd = %d\n", count, fd);
    return count;
}