//
// Created by austinmm on 4/23/19.
//

#include "Level3.h"

int _unmount(char* pathname){

    int md = 0;
    //check if file system is mounted
    if(checkMounted(pathname) != -1){
        print_notice("File system is not mounted");
        return -1;
    }
    //get mount point
    md = get_mount(pathname);
    if(md == -1){
        print_notice("No such mount point");
        return -1;
    }

    //check if mount is busy
    /*
    if(checkBusyFiles(pathname)){
        print_notice("File system is busy");
        return -1;
    }
    */

    MTABLE *m = &_MTables[md];
    m->mntDirPtr->mounted=0; //sets mounted flag to 0
    iput(m->mntDirPtr);
    m->mntDirPtr = NULL;
    MINODE* mip = iget(m->dev-1, m->original_ino); //set back to the old minoded
    iput(mip);
    close(m->dev);
    m->nblocks = 0;
    m->ninodes = 0;
    m->bmap = 0;
    m->imap = 0;
    m->iblock = 0;
    memset(m->devName, 0, 64);
    memset(m->mntName, 0, 64);
    _Total_Mounts--;
    return 0;
}