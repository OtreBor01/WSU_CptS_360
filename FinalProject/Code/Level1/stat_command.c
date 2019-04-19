//
// Created by austinmm on 4/9/19.
//
#include "Level1.h"

int _stat(char* pathname)
{
    /*File: ReadMe.md
    Size: 4421      	Blocks: 16         IO Block: 4096   regular file
    Device: 816h/2070d	Inode: 5116041     Links: 1
    Access: (0644/-rw-r--r--)  Uid: ( 1000/austinmm)   Gid: ( 1000/austinmm)
    Access: 2019-04-09 02:16:33.587404127 -0700
    Modify: 2019-04-09 02:16:20.561426212 -0700
    Change: 2019-04-09 02:16:20.561426212 -0700
    Birth: -
     */
    char* fileType = "unknown";
    int ino = getino(pathname);
    if(ino == 0)
    {
        print_notice("Unable to locate the file specified");
        return -1;
    }
    MINODE* mip = iget(_Running->cwd->dev, ino);
    INODE* ip = &mip->INODE;
    //Get file type name
    if(S_ISDIR(ip->i_mode)){ fileType = "directory"; }
    else if(S_ISREG(ip->i_mode)){ fileType = "regular file"; }
    else if(S_ISLNK(ip->i_mode)){ fileType = "link"; }
    //Get file type permission in string format
    char mode_str[11] = "";
    mode_to_letters(ip->i_mode, mode_str);
    int octalPermission = ip->i_mode & (S_IRWXU | S_IRWXG | S_IRWXO);
    //Print out file stats
    printf("File: %s\n", get_dest_path(pathname));
    printf("Size: %d\tBlocks: %d\tIO Block: %d\t%s\n", ip->i_size, ip->i_blocks, BLKSIZE, fileType);
    printf("Device: %d\tInode: %d\tLinks: %d\n", mip->dev, mip->ino,ip->i_links_count);
    printf("Access: (%04o/%s)\tUid: (%d)\tGid: (%d)\n", octalPermission, mode_str, ip->i_uid,ip->i_gid);
    //Access - the last time the file was read
    time_t t = (time_t)ip->i_atime;
    char *time = ctime(&t);
    if(time == NULL){ time = "-"; }
    else{ time[strlen(time) - 1] = 0;}	//remove \r from time
    printf("Access: %s\n", time);
    //Modify - the last time the file was modified (content has been modified)
    t = (time_t)ip->i_mtime;
    time = ctime(&t); //ctime is the inode or file change time.
    if(time == NULL){ time = "-"; }
    else{ time[strlen(time) - 1] = 0;}	//remove \r from time
    printf("Modify: %s\n", time);
    //Change - the last time meta data of the file was changed (e.g. permissions)
    t = (time_t)ip->i_ctime;
    time = ctime(&t); //ctime is the inode or file change time.
    if(time == NULL){ time = "-"; }
    else{ time[strlen(time) - 1] = 0;}	//remove \r from time
    printf("Change: %s\n", time);
    //Birth time
    t = (time_t)ip->i_dtime;
    time = ctime(&t); //ctime is the inode or file change time.
    if(time == NULL){ time = "-"; }
    else{ time[strlen(time) - 1] = 0;}	//remove \r from time
    printf("Birth: %s\n", time);
    return 0;
}