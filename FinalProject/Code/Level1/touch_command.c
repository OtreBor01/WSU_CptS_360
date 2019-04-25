//
// Created by austinmm on 4/9/19.
//
#include "Level1.h"

int _touch(char* pathname)
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
        print_notice("touch: Unable to locate parent directory");
        return -1;
    }
    MINODE* p_mip = iget(dev, p_ino);
    if(!S_ISDIR(p_mip->INODE.i_mode)){
        print_notice("touch: parent directory is not a valid directory");
        return -1;
    }
    //allocate inode for new file
    int ino = ialloc(dev);
    create_REG_INODE(ino, p_mip);
    int r = enter_name(p_mip, dir, ino, DE_REG);
    //if the file already exist
    if(r == -1){
        //update its access time to now
        int ino = getino(pathname, &dev);
        MINODE* mip = iget(dev, ino);
        time_t now;
        //Access - the last time the file was read
        mip->INODE.i_atime = time(&now);
        printf("The Access Time of '%s' has been Updated to Current Time.", get_dest_path(pathname));
        return ino;
    }
    return r; //returns ino for use by link
}
