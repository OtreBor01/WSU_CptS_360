//
// Created by austinmm on 4/25/19.
//

#include "Level3.h"

int _pmnt(char*pathname)
{
    printf("\n|====================   Total = %d   ====================\n", _Total_Mounts);
    printf("| dev   ninodes   nblocks   iblock   devName   mntName\n");
    printf("| ---   -------   -------   ------   -------   -------\n");
    for(int i = 0; i < NUM_MTABLE; i++){
        MTABLE* mp = &_MTables[i];
        if(mp != NULL && mp->mntDirPtr != NULL)
        {
            printf("| %02d    %07d   %07d   %06d   %7s  %8s\n",
                   mp->dev, mp->ninodes, mp->ninodes, mp->iblock, mp->devName, mp->mntName);
        }
    }
    printf("|=======================================================\n");
    return 0;
}

/*
typedef struct mtable{
  int          dev; // device number; 0 for FREE
  int          ninodes; // from superblock
  int          nblocks;
  int          free_blocks; // from superblock and GD
  int          free_inodes;
  int          bmap; // from group descriptor
  int          imap;
  int          original_ino; //this is the ino value of the inode before it was mounted
  int          iblock; // inodes start block
  MINODE*      mntDirPtr; // mount point DIR pointer
  char         devName[64]; //device name
  char         mntName[64]; // mount point DIR name
}MTABLE;
*/