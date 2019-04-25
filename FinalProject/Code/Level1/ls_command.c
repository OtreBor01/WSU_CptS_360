//
// Created by austinmm on 3/28/19.
//
#include "Level1.h"

int ls_reg(MINODE *mip, char *name)
{
    INODE ip = mip->INODE;

    u16 mode  = ip.i_mode;
    u16 links = ip.i_links_count;
    u16 uid = ip.i_uid;
    u16 gid = ip.i_gid;
    u32 size = ip.i_size;
    //Get Mode
    char mode_str[11] = "";
    mode_to_letters(mode, mode_str);
    //Get Time
    time_t* t = (time_t*)&ip.i_mtime;
    char *time = ctime(t); //ctime is the inode or file change time.
    time[strlen(time) - 1] = 0;	//remove \r from time
    //print the details
    printf("%s %4u %4u %4u %8u %s %s\n", mode_str, links, gid, uid, size, time, name);
    return 0;
}


int ls_dir(MINODE *mip){
    INODE *ip = &mip->INODE;
    for(int i = 0; i < ip->i_blocks; i++){
        int blk = ip->i_block[i];
        if(blk == 0) { break; }
        printf("i_block[%d] = %d\n", i,  blk);
        char buf[BLKSIZE];
        get_block(mip->dev, blk, buf); // get data block into buf[ ]
        DIR *dp = (DIR*)buf; //as dir_entry
        char *cp = buf;
        char temp[PATH_SIZE];
        while(cp < (buf + BLKSIZE))
        {
            strncpy(temp, dp->name, dp->name_len); // make name a string
            temp[dp->name_len] = 0; // ensure NULL at end
            MINODE *temp_mip = iget(mip->dev, dp->inode);
            if(temp_mip->ino == 0){ break; }
            ls_reg(temp_mip, temp);
            iput(temp_mip);
            cp += dp->rec_len; // advance cp by rec_len
            dp = (DIR*)cp; // pull dp to next entry
        }
    }
    return 0;
}

int _ls(char* path)
{
    int dev = _Running->cwd->dev;
    int ino = getino(path, &dev);
    if(ino == 0)
    {
        print_notice("Unable to locate the file specified");
        return -1;
    }
    MINODE *mip = iget(dev, ino);
    if(S_ISDIR(mip->INODE.i_mode))
    {
        ls_dir(mip);
    }
    else if(S_ISREG(mip->INODE.i_mode))
    {
        return 0;
    }
    iput(mip);
    return 0;
}
