//
// Created by austinmm on 4/1/19.
//
#include "Level1.h"

/************** Algorithm of creat *************/
//It is noted that the above creat algorithm differs from Unix/Linux in that..
// it does not open the file for WRITE mode and return a file descriptor.
int _creat(char* pathname)
{
    //This is similar to mkdir() except
    //(1). the INODE.i_mode field is set to REG file type, permission bits set to 0644 = rw-r--r--
    //(2). no data block is allocated for it, so the file size is 0.
    //(3). links_count = 1; Do not increment parent INODE’s liCommand: nks_count
    char* base = get_parent_path(pathname);
    char* dir = get_dest_path(pathname);
    int dev = _Running->cwd->dev;
    int p_ino = getino(base, &dev);
    if(p_ino == 0){
        print_notice("creat: Unable to locate parent directory");
        return -1;
    }
    MINODE* p_mip = iget(dev, p_ino);
    if(!S_ISDIR(p_mip->INODE.i_mode)){
        print_notice("creat: parent directory is not a valid directory");
        return -1;
    }
    //allocate inode for new file
    int ino = ialloc(dev);
    create_REG_INODE(ino, p_mip);
    int r = enter_name(p_mip, dir, ino, DE_REG);
    if(r == -1)
    {
        idalloc(dev, ino);
        print_notice("creat: unable to make duplicate file");
        return -1;
    }
    else if(r == 0)
    {
        idalloc(dev, ino);
        print_notice("creat: failed to add new file to parent directory\n");
        return -1;
    }
    return ino; //returns ino for use by link
}