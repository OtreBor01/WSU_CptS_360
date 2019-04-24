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
    MTABLE *mp = getMountTable(mip->dev);
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
        int block = (i - 1) / 8 + mp->iblock;
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
    return enter_name(p_dest_mip, name, src_mip->ino, fileType);
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
        //improved algorithm
        int read_nbytes = remain;
        if (nbytes < read_nbytes) {
            read_nbytes = nbytes;
        }
        if (bytes_available < read_nbytes){
            read_nbytes = bytes_available;
        }
        memcpy(cp, k_cp, read_nbytes);
        oft->offset += read_nbytes;
        count += read_nbytes;
        nbytes -= read_nbytes;
        bytes_available -= read_nbytes;
        /*
        while (remain > 0){
            // copy bytes from kbuf[ ] to buf[ ]
            *cp++ = *k_cp++;
            oft->offset++; // advance offset
            count++; //incr count
            // dec remain, avail, nbytes;
            remain--; bytes_available--; nbytes--;
            if (nbytes == 0 || bytes_available == 0) { break; }
        }
        */
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
        //improved algorithm
        int write_nbytes = remain;
        if (nbytes < write_nbytes) {
            write_nbytes = nbytes;
        }
        memcpy(k_cp, cp, write_nbytes);
        offset += write_nbytes;
        count += write_nbytes;
        nbytes -= write_nbytes;
        if (offset > mip->INODE.i_size) {  // inc file size
            mip->INODE.i_size += write_nbytes;
        }
        /*
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
        k_buf += 0; //ensures to overwrite exiting buf
        */
        put_block(mip->dev, blk, kbuf);
    }
    // mark minode dirty for iput() when fd is closed
    mip->dirty = 1;
    printf("write: wrote %d char into file descriptor fd = %d\n", count, fd);
    return count;
}

int close_file(int fd)
{
    //Starting to check
    if (_Running->fd[fd] != NULL) { //If file descriptor in process is not null
        _Ofts[fd].refCount--; //decrease refcount of OFT
        if (_Ofts[fd].refCount == 0) {  //If last process using OFT
            iput(_Ofts[fd].minodePtr); //Release minode
        }
    }
    clearOftEntry(fd); //Clear OFT entry
    _Running->fd[fd] = 0; //Clear Process entry
    printf("Closed file descriptor: %d\n",fd);
}

int truncateHelper(MINODE* mip){
    for(int i = 0; i < 14; i++){ //Releases all minode data blocks including double
        //indirect and indirect (12,13)
        int blk = mip->INODE.i_block[i];
        bdalloc(mip->dev,blk);
    }
    time_t now;
    mip->INODE.i_atime=time(&now);//update times
    mip->INODE.i_mtime=time(&now);
    mip->INODE.i_size = 0;
    mip->dirty=1;
}


int open_file(char *filename, int flags){
    //Get files minode
    int ino = getino(filename);
    if(ino == 0){//if file doesnt exist
        ino = _creat(filename); //create file
    }
    MINODE* mip = iget(_Running->cwd->dev, ino);
    //check if file is regular
    if(!S_ISREG(mip->INODE.i_mode)){
        print_notice("open: file type is not regular");
        return -1;
    }

    //Check if file is open in a compatible mode
    OFT* oft = &_Ofts[0];
    int c = 0;
    for(; (c < NUM_OFT) && (oft->minodePtr != NULL) ; c++)
    {
        //If file is already open and has an incompatible type
        if(oft->minodePtr->ino == mip->ino & oft->mode != 0)
        {
            print_notice("open: file already open with incompatible type");
            return -1;
        }
        oft = &_Ofts[c];
    }

    //allocate an openTable entry OFT; initilaize OFT entries
    if(_OpenOFT >= NUM_OFT){
        print_notice("open: unable to allocate anymore space to open new file");
        return -1;
    }
    oft = &_Ofts[_OpenOFT++];
    //Depending on the open mode 0|1|2|3, set the OFT's offset accordingly:
    switch(flags){
        case READ: //0
            oft->offset = 0;
            break;
        case WRITE: //1
            truncateHelper(mip);
            oft->offset = 0;
            break;
        case RDWR: //2
            oft->offset = 0;
            break;
        case APPEND: //3
            oft->offset = mip->INODE.i_size;
            break;
        default:
            print_notice("open: invalid mode entered\n");
            return -1;
    }
    oft->mode = flags;
    oft->refCount =1;
    oft->minodePtr = mip;

    //update INODE's time field
    time_t now;
    oft->minodePtr->INODE.i_atime = time(&now);//update INODE's atime field
    if(flags == 2 || flags == 3 || flags == 1) {//RW APPEND and W
        oft->minodePtr->INODE.i_mtime = time(&now);//update INODE's mtime field
    }
    oft->minodePtr->dirty = 1;//mark node as dirty
    strcpy(oft->fileName, filename);

    //find the SMALLEST i in running PROC's fd[ ] such that fd[i] is NULL
    int i = 0;
    for(; _Running->fd[i] != NULL; i++);
    //Let running->fd[i] point at the OFT entry
    _Running->fd[i] = oft;
    printf("Opened file descriptor: %d\n",i);
    return i;
}