//
// Created by austinmm on 4/18/19.
//

#include "Level1.h"

//checks if a directory inode is empty
int isEmpty(MINODE* mip)
{
    //    verify DIR is empty (traverse data blocks for number of entries = 2);
    char buf[BLKSIZE];
    int blk = mip->INODE.i_block[0];
    get_block(mip->dev, blk, buf);
    DIR* dp = (DIR*)buf;
    char* cp = buf;
    int count = 0;
    for(; cp < (BLKSIZE+buf); count++)
    {
        cp += dp->rec_len;
        dp = (DIR*)cp;
    }
    return count == 2? 1 : 0;
}

//gets the name of the inode  based off of its number: ino and its parent directory: mip
char* get_inode_name(MINODE* mip, int ino){
    INODE* ip = &mip->INODE;
    char buf[BLKSIZE];
    int blk = ip->i_block[0]; //gets the size of the first block index of the inode ip
    get_block(mip->dev, blk, buf); //reads in that blocks data into buf
    DIR* dp = (DIR*)buf; //converts the buf into a ext2_dir_entry_2
    char *cp = buf; //points cp to start of buf
    while(cp < (buf + BLKSIZE)) //while there are still more ext2_dir_entry_2 in the buf to read in
    {
        if(dp->inode == ino) //if the curr ext2_dir_entry_2 number is the same as the one to find
        {
            dp->name[dp->name_len] = 0; // ensure NULL at end
            return dp->name; //returns name of ext2_dir_entry_2 type
        }
        cp += dp->rec_len; // advance cp by rec_len to next ext2_dir_entry_2
        dp = (DIR*)cp; // pull dp to next entry
    }
    return NULL; //did not find the directory
}

//creates a regular file type of inode
int create_REG_INODE(int ino, MINODE* p_mip){
    //The following code segment creates an INODE1⁄4(dev, ino) in a minode, and writes the INODE to disk.
    MINODE *mip = iget(p_mip->dev, ino);
    INODE* ip = &mip->INODE;
    mip->dev = p_mip->dev;
    mip->ino = ino;
    ip->i_mode = S_IFREG; //reg file
    ip->i_uid = _Running->uid; // owner uid
    ip->i_gid = _Running->gid; // group Id
    ip->i_links_count = 1; // links count = 1 because of '.'
    // time_t is arithmetic time type
    time_t now;
    // time() returns the current time of the system as a time_t value
    ip->i_atime = ip->i_ctime = ip->i_mtime = time(&now);;
    //set data blocks to 0
    for(int i = 0; i < 15; i++)
    {
        ip->i_block[i] = 0;
    }
    //set dirty and iput
    mip->dirty = 1;
    iput(mip);
    mip->dirty = 0;
}


//makes a directory mode inode
int kmkdir(MINODE* pmip, char* name){
    int ino, blk;
    char buf[BLKSIZE];


    //Allocate an INODE and disk block
    ino = ialloc(_Root->dev);
    blk = balloc(_Root->dev);

    //4.2 Create INODE
    //Load INODE into a minode
    MINODE* mip = iget(_Root->dev, ino);
    INODE * ip = &mip->INODE;
    ip->i_mode = 0x41ED;// 040755: DIR type and permissions
    ip->i_uid = _Running->uid; // owner uid
    ip->i_gid = _Running->gid; // group Id
    ip->i_size = BLKSIZE; // size in bytes
    ip->i_links_count = 2; // links count=2 because of . and ..
    ip->i_atime = ip->i_ctime = ip->i_mtime = time(0L);
    ip->i_blocks = 2; //LINUX: Blocks count in 512-byte chunks
    ip->i_block[0] = blk; // new DIR has one data block
    for(int i = 1; i<=14;i++){ //set all other data blocks to 0
        ip->i_block[i] = 0;
    }
    mip->dirty = 1; //mark node as dirty
    iput(mip); //write inode to disk

    //4.3
    //Create data block for new DIR containing . and .. entries
    bzero(buf, BLKSIZE);
    DIR* dp = (DIR*) buf;
    //make . entry
    dp->inode = ino;
    dp->rec_len = 12;
    dp->name_len = 1;
    dp->name[0] = '.';
    // make .. entry: pino=parent DIR ino, blk=allocated block
    dp = (char*)dp + 12;
    dp->inode = pmip->ino;
    dp->rec_len = BLKSIZE-12;// rec_len spans block
    dp->name_len = 2;
    dp->name[0] = dp->name[1] = '.';
    put_block(_Root->dev, blk, buf);// write to blk on diks
    return enter_name(pmip, name, ino, DE_DIR);
}

//converts the i_mode value of an inode into a string representation of it
void mode_to_letters(mode_t mode, char *str)
{
    //Get type of mode, i.e. dir, reg, link, etc.
    if (S_ISDIR(mode)) { str[0] = 'd'; }
    else if (S_ISLNK(mode)) { str[0] = 'l'; }
    else { str[0] = '-'; }
    //properties of mode
    str[1] = (mode & S_IRUSR) ? 'r' : '-';
    str[2] = (mode & S_IWUSR) ? 'w' : '-';
    str[3] = (mode & S_IXUSR) ? 'x' : '-';
    str[4] = (mode & S_IRGRP) ? 'r' : '-';
    str[5] = (mode & S_IWGRP) ? 'w' : '-';
    str[6] = (mode & S_IXGRP) ? 'x' : '-';
    str[7] = (mode & S_IROTH) ? 'r' : '-';
    str[8] = (mode & S_IWOTH) ? 'w' : '-';
    str[9] = (mode & S_IXOTH) ? 'x' : '-';
    str[10] = '\0'; //terminating character
}