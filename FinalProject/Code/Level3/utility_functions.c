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
    int ino = getino("/", &dev);
    if(ino == 0){
        print_notice("mount_root: Unable to locate directory specified");
        return NULL;
    }
    MINODE* root = iget(dev, ROOT_INODE); // get root inode
    root->ino = ino;
    if(!S_ISDIR(root->INODE.i_mode)){
        print_notice("mount_root: file specified is not a valid directory");
        return NULL;
    }
    /*
    if(checkBusyFiles()) {
        print_notice("mount_root: directory specified is busy and cannot be mounted at this time");
        return NULL;
    }*/
    if(_Total_Mounts == 1){
        _Root = root;
    }
    return root;

}


int get_mount(char* fs){
    for(int md = 0;md < NUM_MTABLE; md++){
        if(strcmp(_MTables[md].devName, fs)==0){
            return md;
        }
    }
    return -1;
}

int mount_root(char* disk, char* path) {
    //Open Device (disk)
    char device[64] = "../";
    strcat(device, disk);
    int dev = open(device, O_RDWR | O_RDONLY);
    if (dev < 0)
    {
        print_notice("mount_root: Unable to Open Root Device");
        return -1;
    }
    MTABLE* mp = &_MTables[_Total_Mounts++];
    mp->dev = dev;
    strcpy(mp->devName, disk);
    char* dest = get_dest_path(path);
    strcpy(mp->mntName, dest);

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
    int temp_dev = _Running->cwd == NULL? dev : _Running->cwd->dev;
    int ino = getino(path, &temp_dev);
    MINODE* org_mip = iget(temp_dev, ino);
    //Get Root INODE
    MINODE* root = mount_point(path, dev);
    if(root == NULL){
        print_notice("mount_root: unable to locate directory to mount on");
        return -1;
    }
    mp->mntDirPtr = root; // double link
    org_mip->mounted = 1;
    org_mip->mptr = mp;
    printf("|super-magic = %x | bmap = %d | imap = %d | iblock = %d|\n",
           super->s_magic, mp->bmap,  mp->imap, mp->iblock);
    printf("|nblocks = %d | bfree = %d | ninodes = %d | ifree = %d|\n",
           mp->nblocks, mp->free_blocks, mp->ninodes, mp->free_inodes);
    printf("root_mount: '%s' mounted on %s \n", disk, dest);
    return dev;
}
/*
 *  for (int i=0; i<NUM_PROC; i++) // set proc’s CWD
    {
        _Procs[i].cwd = iget(dev, root->ino); // each inc refCount by 1
    }
 */
