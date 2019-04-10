//
// Created by austinmm on 4/1/19.
//
#include "Level1.h"

int create_REG_INODE(int ino, MINODE* p_mip){
    //The following code segment creates an INODE1⁄4(dev, ino) in a minode, and writes the INODE to disk.
    MINODE *mip = iget(p_mip->dev, ino);
    INODE* ip = &mip->INODE;
    mip->dev = p_mip->dev;
    mip->ino = ino;
    ip->i_mode = 33188; //reg file
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
}


/************** Algorithm of creat *************/
//It is noted that the above creat algorithm differs from Unix/Linux in that..
// it does not open the file for WRITE mode and return a file descriptor.
int _creat(char* pathname)
{
    //This is similar to mkdir() except
    //(1). the INODE.i_mode field is set to REG file type, permission bits set to 0644 = rw-r--r--
    //(2). no data block is allocated for it, so the file size is 0.
    //(3). links_count = 1; Do not increment parent INODE’s liCommand: nks_count
    char path[256] = "";
    strcpy(path, pathname);
    char* base = basename(pathname);
    char* dir = dirname(path);
    int dev = _Running->cwd->dev;
    int p_ino = getino(dir);
    if(p_ino == 0){
        printf("Unable to locate directory '%s'\n", dir);
        return -1;
    }
    MINODE* p_mip = iget(dev, p_ino);
    if(!S_ISDIR(p_mip->INODE.i_mode)){
        printf("'%s' is not a valid directory\n", dir);
        return -1;
    }
    int ino = search(p_mip, base);
    if(ino != 0)
    {
        printf("'%s' already exist in '%s'\n", base, dir);
        return -2; //this is -2 so that touch command can know that the file exist and to update the access time
    }
    //allocate inode for new file
    ino = ialloc(dev);
    create_REG_INODE(ino, p_mip);
    ino = enter_name(p_mip, base, ino, EXT2_S_IFREG);
    if(ino == 0){
        printf("Failed to add new file to cwd block\n");
        return -1;
    }
    printf("Successfully Created '%s' in '%s'\n", base, dir);
    return ino; //returns ino for use by link
}