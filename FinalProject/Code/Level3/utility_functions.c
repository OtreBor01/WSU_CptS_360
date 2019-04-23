//
// Created by austinmm on 4/23/19.
//

#include "Level3.h"


int mount_root(char* disk, char* path) {
    //Open Device (disk)
    int dev = open(disk, O_RDWR | O_RDONLY);
    if (dev < 0)
    {
        print_error("mount_root", "Unable to Open Root Device");
    }
    MTABLE* mp = &_MTables[_Total_Mounts++];
    mp->dev = dev;
    strcpy(mp->devName, disk);
    strcpy(mp->mntName, path);


    //Get and check super INODE
    char buf[BLKSIZE];
    get_block(dev, SUPERBLOCK, buf);
    SUPER* super = (SUPER *) buf;
    if (super->s_magic != EXT2_SUPER_MAGIC)
    {
        print_error("mount_root", "Root Device is Not a Valid EXT2 File System");
    }
    mp->ninodes = super->s_inodes_count;
    mp->nblocks = super->s_blocks_count;
    mp->free_blocks = super->s_free_blocks_count;
    mp->free_inodes = super->s_free_inodes_count;

    //Get Group Desc INODE
    get_block(dev, GDBLOCK, buf);
    GD* groupDesc = (GD *)buf;
    mp->bmap = groupDesc->bg_block_bitmap;
    mp->imap = groupDesc->bg_inode_bitmap;
    mp->iblock  = groupDesc->bg_inode_table;

    //Get Root INODE
    int ino = getino(path);
    MINODE* root = iget(dev, ino); // get root inode
    if(_Total_Mounts == 1){
        _Root = root;
    }
    mp->mntDirPtr = root; // double link
    root->mptr = mp;

    printf("|super-magic = %x | bmap = %d | imap = %d | iblock = %d|\n",
           super->s_magic, mp->bmap,  mp->imap, mp->iblock);
    printf("|nblocks = %d | bfree = %d | ninodes = %d | ifree = %d|\n",
           mp->nblocks, mp->free_blocks, mp->ninodes, mp->free_inodes);

    for (int i=0; i<NUM_PROC; i++) // set proc’s CWD
    {
        _Procs[i].cwd = iget(dev, ROOT_INODE); // each inc refCount by 1
    }
    printf("root_mount: '%s' mounted on %s \n", disk, path);
    return dev;
}