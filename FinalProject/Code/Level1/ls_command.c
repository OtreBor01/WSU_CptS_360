//
// Created by austinmm on 3/28/19.
//
#include "Level1.h"

void mode_to_letters(mode_t mode, char *str)
{
    //Get type of mode, i.e. dir, reg, link, etc.
    if (S_ISDIR(mode)) { str[0] = 'd'; }
    else if (S_ISLNK(mode)) { str[0] = 'l'; }
    else { str[0] = '-'; }
    //properties of mode
    str[1] = (mode & S_IRUSR) ? 'r' : '-';
    str[2] = (mode & S_IWUSR) ? 'w' : '-';
    str[3] = (mode & S_IXUSR) ? 'x' : '-';
    str[4] = (mode & S_IRGRP) ? 'r' : '-';
    str[5] = (mode & S_IWGRP) ? 'w' : '-';
    str[6] = (mode & S_IXGRP) ? 'x' : '-';
    str[7] = (mode & S_IROTH) ? 'r' : '-';
    str[8] = (mode & S_IWOTH) ? 'w' : '-';
    str[9] = (mode & S_IXOTH) ? 'x' : '-';
    str[10] = '\0'; //terminating character
}

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
        char temp[256];
        while(cp < (buf + BLKSIZE))
        {
            strncpy(temp, dp->name, dp->name_len); // make name a string
            temp[dp->name_len] = 0; // ensure NULL at end
            MINODE *temp_mip = iget(mip->dev, dp->inode);
            if(temp_mip != NULL)
            {
                ls_reg(temp_mip, temp);
                iput(temp_mip);
            }
            cp += dp->rec_len; // advance cp by rec_len
            dp = (DIR*)cp; // pull dp to next entry
        }
    }
    return 0;
}

int _ls(char* path)
{
    MINODE *mip = _Running->cwd;
    int ino = getino(path);
    if(ino == 0) { return -1; }
    mip = iget(mip->dev, ino);
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
