//
// Created by noah on 4/2/19.
//

#include "Level1.h"

int kmkdir(MINODE* pmip, char* baseName){
    int ino, blk;
    char buf[BLKSIZE];


    //Allocate an INODE and disk block
    ino = ialloc(_Root->dev);
    blk = balloc(_Root->dev);

    //4.2 Create INODE
    //Load INODE into a minode
    MINODE* mip = iget(_Root->dev, ino);
    INODE * ip = &mip->INODE;
    ip->i_mode = 0x41ED;// 040755: DIR type and permissions
    ip->i_uid = _Running->uid; // owner uid
    ip->i_gid = _Running->gid; // group Id
    ip->i_size = BLKSIZE; // size in bytes
    ip->i_links_count = 2; // links count=2 because of . and ..
    ip->i_atime = ip->i_ctime = ip->i_mtime = time(0L);
    ip->i_blocks = 2; //LINUX: Blocks count in 512-byte chunks
    ip->i_block[0] = blk; // new DIR has one data block
    for(int i = 1; i<=14;i++){ //set all other data blocks to 0
        ip->i_block[i] = 0;
    }
    mip->dirty = 1; //mark node as dirty
    iput(mip); //write inode to disk

    //4.3
    //Create data block for new DIR containing . and .. entries
    bzero(buf, BLKSIZE);
    DIR* dp = (DIR*) buf;
    //make . entry
    dp->inode = ino;
    dp->rec_len = 12;
    dp->name_len = 1;
    dp->name[0] = '.';
    // make .. entry: pino=parent DIR ino, blk=allocated block
    dp = (char*)dp + 12;
    dp->inode = pmip->ino;
    dp->rec_len = BLKSIZE-12;// rec_len spans block
    dp->name_len = 2;
    dp->name[0] = dp->name[1] = '.';
    put_block(_Root->dev, blk, buf);// write to blk on diks
    enter_name(pmip,baseName,ino, EXT2_S_IFDIR);
}


int _mkdir(char* pathname){
    char basePathName[100], dirPathName[100];
    int pino;
    MINODE * pmip;
    //Grab dir name and path name
    strcpy(basePathName, basename(pathname));
    strcpy(dirPathName, dirname(pathname));
    //Grab ino to check if dir exist and is a DIR type
    pino = getino(dirPathName);
    if(pino == 0){
        print_notice("Directory does not exist");
        return -1;
    }
    pmip = iget(_Root->dev, pino);
    if(!S_ISDIR(pmip->INODE.i_mode)){
        print_notice("Type of INODE is not DIR");
        return -1;
    }
    if(search(pmip,basePathName) != 0){
        print_notice("Base name already exist");
        return -1;
    }

    kmkdir(pmip,basePathName);
    pmip->INODE.i_links_count = 1;
    pmip->dirty = 1;
    iput(pmip);
}
