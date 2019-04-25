//
// Created by austinmm on 4/7/19.
//

#include "Level1.h"

/*
 Every DIR’s links_count starts with 2 (for the. and .. entries). Each
subdirectory increments its links_count by 1 but regular files do not increment the links_count of
the parent directory. So, if a DIR’s links_count is greater than 2, it is definitely not empty. However,
if links_count 1⁄4 2, the DIR may still contain regular files. In that case, we must traverse the DIR’s
data blocks to count the number of dir_entries, which must be greater than 2.
 */

int _rmdir(char* pathname)
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

    if(check_file_permissions(mip) == -1 & _Su == 0 & _Sudo == 0){//check file permissions
        print_notice("You do not have rights to this file");
        iput(mip);
        return -1;
    }

    if(mip->refCount > 1) //minode is not BUSY (refCount = 1);
    {
        print_notice("Cannot rmdir because directory is currently being used");
        iput(mip);
        return -1;
    }
    if(!S_ISDIR(mip->INODE.i_mode))//minode is not BUSY (refCount = 1);
    {
        print_notice("Cannot rmdir because file is not a valid directory");
        iput(mip);
        return -1;
    }
    if(!isEmpty(mip))
    {
        print_notice("Cannot rmdir because directory is not empty");
        iput(mip);
        return -1;
    }

    //(3). get parent’s ino and inode
    int pino = search(mip, "..", &dev); //get parent inode number by getting pino from .. entry in INODE.i_block[0]
    MINODE* pmip = iget(dev, pino); //get parent directory MINODE

    //(4). get name from parent DIR’s data block
    char* bname = get_dest_path(pathname); //gets the name of the file to delete, findname(pmip, ino, name);

    //(5). remove name from parent directory
    int isDir = 1; //ensures that file with the same name as dir doesn't get removed
    int didWork = remove_name(pmip, bname, isDir);
    if(didWork == 0)
    {
        print_error("_rmdir", "Failed to locate and remove directory specified");
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
