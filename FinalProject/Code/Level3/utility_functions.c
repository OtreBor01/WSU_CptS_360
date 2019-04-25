//
// Created by austinmm on 4/23/19.
//

#include "Level3.h"
/*
 * prints the mounted file systems and the directories they are mounted to
 */
int mountStat(void){
    for(int i = 0; i < _Total_Mounts; i++){
        printf("%s is mounted on %s\n", _MTables[i].devName, _MTables[i].mntName);
    }
}


/*
 * Returns 1 if mounted and 0 if not mounted
 */
int checkMounted(char* diskname){
    int i = 0;
    for(;(i < _Total_Mounts) & (strlen(_MTables[i].devName) != 0); i++){
        if(strcmp(_MTables[i].devName, diskname) == 0){
            return -1;
        }
    }
    return 0;
}

/*
 *Checks if a path is currently a processes cwd
 */
int checkCwdBusy(char* pathname){
    int i = 0;
    for(;i < NUM_PROC & strlen(_Procs[i].cwd)!= 0; i++){
        if(strcmp(_Procs[i].cwd,pathname)==0){
            print_notice("Directory is currently busy");
            return -1;
        }
    }
    return 0;
}

int checkBusyFiles(char* dev){
    for (int i = 0; i < NUM_MINODE; i++){
        if(strcmp(_MINodes[i].dev,dev)==0){ //might cause bug because its attempting to check a string with an int representation of dev
            return 1;
        }
    }
    return 0;
}

MINODE* mount_point(char* path, int dev){


}


int get_mount(char* fs){
    for(int md = 0;md < NUM_MTABLE; md++){
        if(strcmp(_MTables[md].devName, fs)==0){
            return md;
        }
    }
    return -1;
}

SUPER* getSuper(MTABLE* mp, int dev){
    //Get and check super INODE
    char buf[BLKSIZE];
    get_block(dev, SUPERBLOCK, buf);
    SUPER* super = (SUPER *) buf;
    if (super->s_magic != EXT2_SUPER_MAGIC)
    {
        print_notice("mount_root: device is Not a Valid EXT2 File System");
        return NULL;
    }
    mp->ninodes = super->s_inodes_count;
    mp->nblocks = super->s_blocks_count;
    mp->free_blocks = super->s_free_blocks_count;
    mp->free_inodes = super->s_free_inodes_count;
    return super;
}

GD* getGroupDesc(MTABLE* mp, int dev){
    //Get Group Desc INODE
    char buf[BLKSIZE];
    get_block(dev, GDBLOCK, buf);
    GD* groupDesc = (GD *)buf;
    mp->bmap = groupDesc->bg_block_bitmap;
    mp->imap = groupDesc->bg_inode_bitmap;
    mp->iblock  = groupDesc->bg_inode_table;
    return groupDesc;
}

MINODE* getRoot(MTABLE* mp, int dev, int ino){
    MINODE* mip = iget(dev, ROOT_INODE);
    if(!S_ISDIR(mip->INODE.i_mode)){
        print_notice("mount_root: file specified is not a valid directory");
        return NULL;
    }
    mp->original_ino = ino;
    mp->mntDirPtr = mip;
    mip->mptr = mp;
    mip->mounted = 1;
    return mip;
}

int get_original_ino(char* path){
    int original_ino = 2;
    if (_Running->cwd != NULL) {
        int dev = _Running->cwd->dev;
        original_ino = getino(path, &dev);
        if (original_ino == 0) {
            print_notice("mount_root: could not locate file to mount onto");
            return -1;
        }
    }
    return original_ino;
}


int open_device(char* disk){
    char device[64] = "../";
    strcat(device, disk);
    int dev = open(device, O_RDWR | O_RDONLY);
    if (dev < 0)
    {
        print_notice("mount_root: Unable to Open Root Device");
        return -1;
    }
    return dev;
}


int mount_root(char* disk, char* path) {

    //gets the ino number of the mip that is about to be mounted to new disk
    int original_ino = get_original_ino(path);

    int dev = open_device(disk);
    if(dev == -1){ return -1; }

    //Gets index of mtable to mount onto
    MTABLE* mp = &_MTables[_Total_Mounts];
    mp->dev = dev;
    strcpy(mp->devName, disk);
    char* dest = get_dest_path(path);
    strcpy(mp->mntName, dest);

    //Super
    SUPER* super = getSuper(mp, dev);
    if(super == NULL){ return -1; }

    //Group
    GD* groupDesc = getGroupDesc(mp, dev);
    if(groupDesc == NULL){ return -1; }

    //Get directory (MINODE*) to mount onto
    MINODE* mip = getRoot(mp, dev, original_ino);
    if(mip == NULL){ return -1; }

    if(_Total_Mounts == 0) {
        _Root = mip;
    }

    printf("|super-magic = %x | bmap = %d | imap = %d | iblock = %d|\n",
           super->s_magic, mp->bmap,  mp->imap, mp->iblock);
    printf("|nblocks = %d | bfree = %d | ninodes = %d | ifree = %d|\n",
           mp->nblocks, mp->free_blocks, mp->ninodes, mp->free_inodes);
    printf("root_mount: '%s' mounted on %s \n", disk, dest);
    _Total_Mounts++;
    return dev;
}
/*
 *  for (int i=0; i<NUM_PROC; i++) // set proc’s CWD
    {
        _Procs[i].cwd = iget(dev, root->ino); // each inc refCount by 1
    }
 */
