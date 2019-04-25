//
// Created by noah on 4/7/19.
//
#include "Level1.h"

int _symlink(char* pathname){
    char* oldFile ,*newFile,  *parentDirName, *childBaseName;
    int oino, nino, pino; //old inode and new inode and parent inode
    MINODE * omip , *pmip, *nmip; //old mip and parent mip

    printf("\n\npathname: %s\n\n",pathname);
    //parse path name
    oldFile = strtok(pathname, " "); //old File pathname
    newFile = strtok(0, " "); //new File pathname

    printf("oldFile:  %s\n\n",oldFile);
    printf("newFile:  %s\n\n",newFile);
    //get old ino and minode
    int dev = _Running->cwd->dev;
    oino = getino(oldFile, &dev);
    //Verify old file exist and is right type
    if (oino == 0){
        print_notice("Old file does not exist");
        return -1;
    }
    omip = iget(dev, oino);
    if(S_ISDIR(omip->INODE.i_mode)){
        printf("Error: File Type must not be DIR");
        return -1;
    }
    //get new ino
    nino = getino(newFile, &dev);
    //Verify newFile does not exist
    if(nino != 0){
        printf("Error: New File must not exist yet");
        return -1;
    }

    //create new file for link and grab ino
    nino = _creat(newFile);
    pino = getino(dirname(newFile), &dev);
    pmip = iget(dev,pino);
    nmip = iget(dev,nino);
    INODE * ip = &nmip->INODE;
    ip->i_mode = S_IFLNK;

    //store oldFile name into newFile INODE blocks area


    int i = 0;
    for(i; i < strlen(oldFile);i++){
        ip->i_block[i] = oldFile[i];
    }

    //ip->i_block[i+1] = basename(oldFile)[i+1];


    //Set file size to be size of oldFile name
    nmip->INODE.i_size = strlen(oldFile);
    nmip->dirty = 1;
    //Write to disk
    iput(nmip);

    //Mark parent as dirty
    pmip->dirty=1;
    //write parent to disk
    iput(pmip);
    return 0;

}