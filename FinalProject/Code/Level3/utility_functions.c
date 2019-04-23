//
// Created by austinmm on 4/23/19.
//

#include "Level3.h"


int mount_root(char* disk, char* path) {
    int start_block = 0;
    int num_of_blocks = 0;
    SUPER* super;
    GD* groupDesc;
    //Open Device (disk)
    int dev = open(disk, O_RDWR | O_RDONLY);
    if (dev < 0)
    {
        print_error("mount_root", "Unable to Open Root Device");
    }

    //Get and check super INODE
    char buf[BLKSIZE];
    get_block(dev, SUPERBLOCK, buf);
    super = (SUPER *) buf;
    if (super->s_magic != EXT2_SUPER_MAGIC)
    {
        print_error("mount_root", "Root Device is Not a Valid EXT2 File System");
    }

    //Get Group Desc INODE
    get_block(dev, GDBLOCK, buf);
    groupDesc = (GD *)buf;

    //Get Root INODE
    MINODE* root = iget(dev, path); // get root inode
    if(_Total_Mounts == 0){
        _Root = root;
    }


    // fill mount table mtable with disk information
    MTABLE* mp = &_MTables[_Total_Mounts++];
    mp->dev = dev;
    mp->ninodes = super->s_inodes_count;
    mp->nblocks = super->s_blocks_count;
    mp->free_blocks = super->s_free_blocks_count;
    mp->free_inodes = super->s_free_inodes_count;
    strcpy(mp->devName, disk);
    strcpy(mp->mntName, path);
    mp->bmap = groupDesc->bg_block_bitmap;
    mp->imap = groupDesc->bg_inode_bitmap;
    mp->iblock  = groupDesc->bg_inode_table;
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