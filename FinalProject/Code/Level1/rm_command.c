//
// Created by austinmm on 4/7/19.
//

#include "Level1.h"


int _rm(char* pathname)
{
    //(1). get in-memory INODE of pathname:
    int dev = _Running->cwd->dev;
    int ino = getino(pathname, &dev);
    if(ino == 0)
    {
        print_notice("Unable to locate the file specified");
        return -1;
    }
    MINODE* mip = iget(dev, ino);

    //(2). verify INODE is a DIR (by INODE.i_mode field);
    if(mip->refCount > 1) //minode is not BUSY (refCount = 1);
    {
        print_notice("Cannot rm because file is currently being used");
        return -1;
    }
    if(S_ISDIR(mip->INODE.i_mode))//minode is not BUSY (refCount = 1);
    {
        print_notice("Cannot rm because file is a directory");
        return -1;
    }

    //(3). get parent’s ino and inode
    char* base = get_parent_path(pathname);
    int pino = getino(base, &dev); //get parent inode number by getting pino from .. entry in INODE.i_block[0]
    MINODE* pmip = iget(dev, pino); //get parent directory MINODE

    //(4). get name from parent DIR’s data block
    char* name = get_dest_path(pathname); //gets the name of the file to delete, findname(pmip, ino, name);

    //(5). remove name from parent directory
    int isDir = 0;
    int didWork = remove_name(pmip, name, isDir);
    if(didWork == 0)
    {
        print_error("_rm", "Failed to locate and remove file specified");
        return -1;
    }

    //(6). dec parent links_count by 1; mark parent pimp dirty;
    iput(pmip);

    //(7). deallocate its data blocks and inode
    bdalloc(mip->dev, mip->INODE.i_block[0]);
    idalloc(mip->dev, mip->ino);
    iput(mip);

    return 0;
}
