//
// Created by austinmm on 3/28/19.
//
#include "Level1.h"

char* get_inode_name(MINODE* mip, int ino){
    INODE* ip = &mip->INODE;
    char buf[BLKSIZE];
    int blk = ip->i_block[0]; //gets the size of the first block index of the inode ip
    get_block(mip->dev, blk, buf); //reads in that blocks data into buf
    DIR* dp = (DIR*)buf; //converts the buf into a ext2_dir_entry_2
    char *cp = buf; //points cp to start of buf
    while(cp < (buf + BLKSIZE)) //while there are still more ext2_dir_entry_2 in the buf to read in
    {
        if(dp->inode == ino) //if the curr ext2_dir_entry_2 number is the same as the one to find
        {
            dp->name[dp->name_len] = 0; // ensure NULL at end
            return dp->name; //returns name of ext2_dir_entry_2 type
        }
        cp += dp->rec_len; // advance cp by rec_len to next ext2_dir_entry_2
        dp = (DIR*)cp; // pull dp to next entry
    }
    return NULL; //did not find the directory
}

int r_pwd(int depth, char *path, MINODE* wd)
{
    //Base case
    if (wd == _Root) { return depth; }
    int ino = search(wd, ".."); //get parent inode number
    MINODE* pd = iget(wd->dev, ino); //get parent directory MINODE
    char* name = get_inode_name(pd, wd->ino); //gets the name of the 'wd'
    if(name == NULL){ return -1; }
    char temp[256] = "";
    //Builds the pwd string
    int isEmpty = !strcmp(path, "");
    strcpy(temp, path);
    strcpy(path, "/");
    strcat(path, name);
    if(!isEmpty){
        strcat(path, temp);
    }
    //recursive call to next parent
    return r_pwd(depth + 1, path, pd);

}

int _pwd(MINODE* wd)
{
    char path[256] = "";
    int depth = r_pwd(0, path, wd);
    if(depth == 0){
        strcpy(path, "/");
    }
    puts(path);
}