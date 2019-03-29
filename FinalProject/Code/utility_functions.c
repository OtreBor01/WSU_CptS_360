// Created by austinmm on 3/28/19.

#include "Header.h"

//Function Declarations

void print_error(char* function, char* message)
{
    printf("Error: %s -> %s\n", function, message);
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
            printf("%8d%8d%8u %s\n", dp->inode, dp->rec_len, dp->name_len, temp);
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
    for (int i=0; i<NMINODE; i++)
    {
        MINODE* mp = &_MINode[i];
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
    MTABLE* mp;
    char buf[BLKSIZE];
    // serach in-memory minodes first
    for(int i = 0; i < NMINODE; i++)
    {
        mip = &_MINode[i];
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
    int offset= (ino - 1) % 8; // which inode in this block
    get_block(dev, block, buf);
    INODE* ip = (INODE *)buf + offset;
    mip->INODE = *ip; // copy inode to minode.INODE
    // initialize minode
    mip->refCount = 1;
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
    int ino;
    for (int i = 0; i<_PathTokenCount; i++) // search for each component string
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