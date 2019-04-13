// Created by austinmm on 3/28/19.

#include "Header.h"

//Function Declarations

void print_error(char* function, char* message)
{
    printf("Error: %s -> %s\n", function, message);
    exit(1);
}

void print_notice( char* message)
{
    printf("Notice: %s\n", message);
}

int tokenize(char *pathname)
{
    strcpy(_Path, pathname);
    _PathTokenCount = 0;
    char* token = strtok(pathname, PATH_DELIMITER);
    while(token){
        _PathTokens[_PathTokenCount++] = token;
        token = strtok(NULL, PATH_DELIMITER);
    }
}

int search(MINODE* mip, char* name)
{
    for (int i = 0; i < 12; i++)  // search DIR direct blocks only
    {
        char temp[256], buf[BLKSIZE];
        int blk = mip->INODE.i_block[i];
        if (blk == 0) { return 0; }
        get_block(mip->dev, blk, buf);
        DIR *dp = (DIR *)buf;
        char* cp = buf;
        while (cp < (buf + BLKSIZE)){
            strncpy(temp, dp->name, dp->name_len);
            temp[dp->name_len] = 0;
            //printf("%8d%8d%8u %s\n", dp->inode, dp->rec_len, dp->name_len, temp);
            if (!strcmp(name, temp)){
                printf("Found '%s': inumber = %d\n", name, dp->inode);
                return dp->inode;
            }
            cp += dp->rec_len;
            dp = (DIR *)cp;
        }
    }
    return 0;
}

MINODE* mialloc(void) // allocate a FREE minode for use
{
    for (int i=0; i<NUM_MINODE; i++)
    {
        MINODE* mp = &_MINodes[i];
        if (mp->refCount == 0)
        {
            mp->refCount = 1;
            return mp;
        }
    }
    print_error("mialloc", "Unable to allocate any more minodes");
    return 0;
}

int midalloc(MINODE *mip) // release a used minode
{
    mip->refCount = 0;
}

int get_block(int dev, int blk, char* buf)
{
    lseek(dev, blk * BLKSIZE, SEEK_SET);
    int n = read(dev, buf, BLKSIZE);
    if (n < 0){
        print_error("get_block", "Failure to read in new Block");
    }
    return n;
}

int put_block(int dev, int blk, char* buf)
{
    lseek(dev, blk * BLKSIZE, SEEK_SET);
    int n = write(dev, buf, BLKSIZE);
    if (n != BLKSIZE) {
        print_error("put_block", "Failure to write Block");
    }
    return n;
}


/*iget(dev, ino) function:
    This function returns a pointer to the in-memory minode containing the INODE of (dev, ino).
    The returned minode is unique, i.e. only one copy of the INODE exists in memory.
    In a real file system, the returned minode is locked for exclusive use until it is either released or unlocked.
    For simplicity, we shall assume that minode locking is unnecessary, which will be explained later.*/
MINODE *iget(int dev, int ino)
{
    MINODE* mip;
    // serach in-memory minodes first
    for(int i = 0; i < NUM_MINODE; i++)
    {
        mip = &_MINodes[i];
        if (mip->refCount
            && (mip->dev == dev)
            && (mip->ino == ino))
        {
                mip->refCount++;
                return mip;
        }
    }
    // needed INODE=(dev,ino) not in memory
    mip = mialloc(); // allocate a FREE minode
    // assign to (dev, ino)
    mip->dev = dev;
    mip->ino = ino;
    int block = (ino - 1) / 8 + _IStartBlock; // disk block containing this inode
    int offset = (ino - 1) % 8; // which inode in this block
    char buf[BLKSIZE];
    get_block(dev, block, buf);
    INODE* ip = (INODE *)buf + offset;
    mip->INODE = *ip; // copy inode to minode.INODE
    // initialize minode
    mip->mounted = 0;
    mip->dirty = 0;
    mip->mptr = 0;
    return mip;
}


/*iput(INODE *mip) function:
    This function releases a used minode pointed by mip.
    Each minode has a refCount, which represents the number of users that are using the minode.
    iput() decrements the refCount by 1.
    If the refCount is non-zero, meaning that the minode still has other users, the caller simply returns.
    If the caller is the last user of the minode (refCount 1⁄4 0), the INODE is written back to disk if it is modified (dirty).*/
int iput(MINODE *mip) {
    char buf[BLKSIZE];
    if (mip == 0) { return 0; }
    mip->refCount--; // dec refCount by 1
    if (mip->refCount > 0) { return 0; } // still has user
    if (mip->dirty == 0) { return 0; } // no need to write back
    // write INODE back to disk
    int block = (mip->ino - 1) / 8 + _IStartBlock;
    int offset = (mip->ino - 1) % 8;
    // get block containing this inode
    get_block(mip->dev, block, buf);
    INODE* ip = (INODE *)buf + offset; //ip points at INODE
    *ip = mip->INODE; //copy INODE to inode in block
    put_block(mip->dev, block, buf); //write back to disk
    midalloc(mip); //mip->refCount = 0;
}

/*getino() function:
    The getino() function implements the file system tree traversal algorithm.
    It returns the INODE number (ino) of a specified pathname.
    To begin with, we assume that in the level-1 file system implementation,
    the file system resides on a single root device, so that there are no mounted devices and mounting point crossings.
    Mounted file systems and mounting point crossing will be considered later in level-3 of the file system implementation.
    Thus, the getino() function essentially returns the (dev, ino) of a pathname.
    The function first uses the tokenize() function to break up pathname into component strings.
    We assume that the tokenized strings are in a global data area, each pointed by a name[i] pointer and the number of token strings is nname.
    Then it calls the search() function to search for the token strings in successive directories.
    The following shows the tokenize() and search() functions.*/
int getino(char *pathname)
{
    MINODE *mip;
    if (!strcmp(pathname, "/")){
        return 2; // return root ino = 2
    }
    else if (pathname[0] == '/') {
        mip = _Root; //if absolute pathname: start from root
    }
    else {
        mip = _Running->cwd; //if relative pathname: start from CWD
    }
    mip->refCount++; //in order to iput(mip) later
    tokenize(pathname); // assume: name[ ], nname are globals
    int ino = mip->ino;
    for (int i = 0; i < _PathTokenCount; i++) // search for each component string
    {
        char* name = _PathTokens[i];
        if (!S_ISDIR(mip->INODE.i_mode)) // check DIR type
        {
            printf("'%s' is Not a Directory\n", name);
            iput(mip);
            return 0;
        }
        ino = search(mip, name);
        if (ino == 0)
        {
            printf("No such Component named '%s'\n", name);
            iput(mip);
            return 0;
        }
        iput(mip); // release current minode
        mip = iget(mip->dev, ino); // switch to new minode
    }
    iput(mip);
    return ino;
}



int enter_name(MINODE* p_mip, char* base, int ino, int fileType)
{
    int block_index = 0;
    while(1)
    {
        char buf[BLKSIZE];
        int blk = p_mip->INODE.i_block[block_index];
        if (blk == 0) { return 0; }
        get_block(p_mip->dev, blk, buf);
        DIR *dp = (DIR *) buf;
        char *cp = buf;
        //All dir_entries rec_len = ideal_length, except the last entry.
        //The rec_len of the LAST entry is to the end of the block, which may be larger than its ideal_length.
        int name_len = strlen(base);
        //In order to enter a new entry of name with n_len, the needed length is
        int need_length = 4 * ((8 + name_len + 3) / 4);
        while ((cp + dp->rec_len) < (buf + BLKSIZE))
        {
            cp += dp->rec_len;
            dp = (DIR *) cp;
        }
        /*if(dp == NULL){
            decFreeInodes//The new data block containins only one entry.
            dp->rec_len = BLKSIZE;
        }*/
        // dp NOW points at last entry in block
        int ideal_length = 4 * ((8 + dp->name_len + 3) / 4);
        int remain = dp->rec_len - ideal_length;
        if (remain >= need_length) {
            //trim the previous entry rec_len to its ideal_length;
            strncpy(dp->name, dp->name, ideal_length);
            dp->rec_len = ideal_length;
            dp->name[dp->rec_len] = 0;
            //enter the new entry as the LAST entry and
            cp += dp->rec_len;
            dp = (DIR *) cp;
            dp->rec_len = remain;
            dp->file_type = fileType;
            dp->name_len = name_len;
            dp->inode = ino;
            strcpy(dp->name, base);
            put_block(p_mip->dev, blk, buf);
            return dp->inode;
        }
        else if (block_index < 12){
            if((p_mip->INODE.i_blocks - 1) <= ++block_index){
                p_mip->INODE.i_size += BLKSIZE;
                p_mip->INODE.i_blocks++;
            }
        }
        else{
            print_notice("Cannot Allocate Anymore Blocks to INODE");
            break;
        }
    }
}

int incFreeInodes(int dev)
{
    char buf[BLKSIZE];
    // dec free inodes count in SUPER and GD
    get_block(dev, 1, buf); //get super
    _Super = (SUPER *)buf;
    _Super->s_free_inodes_count++;
    put_block(dev, 1, buf); //update super
    get_block(dev, 2, buf); //group descipter
    _GroupDec = (GD *)buf;
    _GroupDec->bg_free_inodes_count++;
    put_block(dev, 2, buf);
}

int decFreeInodes(int dev)
{
    char buf[BLKSIZE];
    // dec free inodes count in SUPER and GD
    get_block(dev, 1, buf); //get super
    _Super = (SUPER *)buf;
    _Super->s_free_inodes_count--;
    put_block(dev, 1, buf); //update super
    get_block(dev, 2, buf); //group descipter
    _GroupDec = (GD *)buf;
    _GroupDec->bg_free_inodes_count--;
    put_block(dev, 2, buf);
}


int ialloc(int dev)
{
    char buf[BLKSIZE];
    // use imap, ninodes in mount table of dev
    MTABLE *mp = &_MTables[0];
    get_block(dev, mp->imap, buf);
    for (int i=0; i < mp->ninodes; i++)
    {
        if (tst_bit(buf, i) == 0){
            set_bit(buf, i);
            put_block(dev, mp->imap, buf);
            // update free inode count in SUPER and GD
            decFreeInodes(dev);
            return (i+1);
        }
    }
    return 0; // out of FREE inodes
}

/* deallocates an inode (number). It clears the ino’s bit in the device’s inodes bitmap to 0. Then it
increments the free inodes count in both superblock and group descriptor by 1.*/
int idalloc(int dev, int ino) {
    char buf[BLKSIZE];
    MTABLE *mp = &_MTables[0];
    if (ino > mp->ninodes){ // niodes global
        print_notice("INode number is out of range");
        return -1;
    }
    // get inode bitmap block
    get_block(dev, mp->imap, buf);
    clr_bit(buf, ino-1);
    // write buf back
    put_block(dev, mp->imap, buf);
    // update free inode count in SUPER and GD
    incFreeInodes(dev);
}


int tst_bit(char *buf, int bit)
{
    int i = bit / 8;
    int j = bit % 8;
    if (buf[i] & (1 << j)) {
        return 1;
    }
    return 0;
}

int set_bit(char *buf, int bit)
{
    int i = bit / 8;
    int j = bit % 8;
    buf[i] |= (1 << j);
}

int clr_bit(char *buf, int bit)
{
    int i = bit / 8;
    int j = bit % 8;
    buf[i] &= ~(1 << j);
}

int incFreeBlocks(int dev)
{
// dec free inodes count in SUPER and GD
    char buf[BLKSIZE];
    get_block(dev, 1, buf);
    _Super = (SUPER *)buf;
    _Super->s_free_blocks_count;
    put_block(dev, 1, buf);
    get_block(dev, 2, buf);
    _GroupDec = (GD *)buf;
    _GroupDec->bg_free_blocks_count++;
    put_block(dev, 2, buf);
}


int decFreeBlocks(int dev)
{
// dec free inodes count in SUPER and GD
    char buf[BLKSIZE];
    get_block(dev, 1, buf);
    _Super = (SUPER *)buf;
    _Super->s_free_blocks_count;
    put_block(dev, 1, buf);
    get_block(dev, 2, buf);
    _GroupDec = (GD *)buf;
    _GroupDec->bg_free_blocks_count--;
    put_block(dev, 2, buf);
}

int balloc(int dev)
{
    char buf[BLKSIZE];
    // use bmap, nblocks in mount table of dev
    //MTABLE *mp = (MTABLE *)get_mtable(dev);
    MTABLE *mp = &_MTables[0];
    get_block(dev, mp->bmap, buf);
    for (int i=0; i < mp->nblocks; i++){
        if (tst_bit(buf, i)==0){
            set_bit(buf, i);
            put_block(dev, mp->bmap, buf);
            // update free blocks count in SUPER and GD
            decFreeBlocks(dev);
            return (i+1);
        }
    }
    return 0; // out of FREE blocks
}


//function which deallocates a disk block (number) bno.
int bdalloc(int dev, int bno)
{
    char buf[BLKSIZE];
    MTABLE *mp = &_MTables[0];
    if (bno > mp->nblocks){
        print_notice("Block-Node number is out of range");
        return -1;
    }
    // get block map
    get_block(dev, mp->bmap, buf);
    clr_bit(buf, bno-1);
    // write buf back
    put_block(dev, mp->bmap, buf);
    // update free iblocks count in SUPER and GD
    incFreeBlocks(dev);
}

//deallocate a block
//once deallocated we also increment the number of free blocks
int bdealloc(int dev, int bno)
{
    char buf[1024];
    int byte;
    int bit;
    MTABLE *mp = &_MTables[0];
    //clear bit(bmap, bno)
    get_block(dev, mp->bmap, buf);

    byte = bno / 8;
    bit = bno % 8;

    buf[byte] &= ~(1 << bit);

    put_block(dev, mp->bmap, buf);

    //set free blocks
    get_block(dev, 1, buf);
    _Super = (SUPER *)buf;
    _Super->s_free_blocks_count++;
    put_block(dev, 1, buf);

    get_block(dev, 2, buf);
    _GroupDec = (GD *)buf;
    _GroupDec->bg_free_blocks_count++;
    put_block(dev, 2, buf);

    return 0;
}

//deallocates an inode for a given ino on the dev
//This is used when we remove things
//Once dealocated, we increment the free inodes in the SUPER and in the group descriptor
int idealloc(int dev, int ino)
{
    char buf[1024];
    int byte;
    int bit;
    MTABLE *mp = &_MTables[0];
    //clear bit(bmap, bno)
    get_block(dev, mp->imap, buf);

    //Mailmans to where it is
    byte = ino / 8;
    bit = ino % 8;

    //Negate it
    buf[byte] &= ~(1 << bit);

    put_block(dev, mp->imap, buf);

    //set free blocks
    get_block(dev, 1, buf);
    _Super = (SUPER *)buf;
    _Super->s_free_blocks_count++;
    put_block(dev, 1, buf);

    get_block(dev, 2, buf);
    _GroupDec = (GD *)buf;
    _GroupDec->bg_free_blocks_count++;
    put_block(dev, 2, buf);
}