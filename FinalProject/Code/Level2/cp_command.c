//
// Created by austinmm on 4/18/19.
//

//
// Created by austinmm on 4/17/19.
//

#include "Level2.h"


int _cp(char*pathname)
{
    //1. ***** Seperates Paths into Source and Dest *****
    char source[125] = "", dest[125] = "";
    int validPaths = seperate_path(pathname, source, dest);
    if(validPaths == -1){ return -1; }

    //2. ***** Get ino values for the base and dir files specified by source and dest paths
    //get inode numbers of files specified to move to and from
    int src_ino = getino(source);

    //get inode numbers of parent directories of files specified to move to and from..
    // checks if the parent directories are the root dir as well in the ternary operator
    char* base = get_parent_path(source);
    int p_src_ino = getino(base);

    //ensures that both the dir and base files exist according to the source pathname provided
    if(src_ino == 0 || p_src_ino == 0){
        print_notice("cp: unable to locate file operand");
        return -1;
    }

    base = get_parent_path(dest);
    int p_dest_ino = getino(base);
    MINODE* p_dest_mip = iget(_Running->cwd->dev, p_dest_ino);
    if(p_dest_ino == 0 || p_dest_mip == NULL || !S_ISDIR(p_dest_mip->INODE.i_mode)){
        print_notice("mv: unable to locate destination file operand");
        return -1;
    }


    //fd = open src for READ;
    char openRead[PATH_SIZE] = "";//read mode
    strcpy(openRead, source);
    strcat(openRead, " 0");
    int fd = _open(openRead);

    //gd = open dst for WR|CREAT;
    char openWrite[PATH_SIZE] = "";//write mode
    strcpy(openWrite, dest);
    strcat(openWrite, " 1");
    int gd = _open(openWrite);

    _pfd("");
    char buf[BLKSIZE];
    int nbytes = 0;
    //loop this?
    nbytes = read_file(fd, buf, BLKSIZE);
    nbytes = write_file(gd, buf, nbytes);  // notice the nbytes in write()
    //close the files
    char fd_str[3] = "";
    snprintf(fd_str, sizeof(fd_str), "%d", fd);
    _close(fd_str);
    snprintf(fd_str, sizeof(fd_str), "%d", gd);
    _close(fd_str);
    return nbytes;
}