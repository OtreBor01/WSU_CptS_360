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
    int fd = open_file(source, 0);
    if(fd < 0){
        print_notice("cp: unable to open source file for read");
        return -1;
    }
    //gd = open dst for WR|CREAT;
    int gd = open_file(dest, 1);
    if(gd < 0){
        print_notice("cp: unable to open destination file for write");
        return -1;
    }

    char buf[BLKSIZE];
    int nbytes = 0;
    //copies all bytes from source file to dest file
    while((nbytes = read_file(fd, buf, BLKSIZE)) > 0){
        buf[nbytes] = 0;             // as a null terminated string
        //spit out chars from buf[ ] but handle \n properly
        nbytes = write_file(gd, buf, nbytes);  // notice the nbytes in write()
    }
    //close the files
    close_file(fd);
    close_file(gd);
    return nbytes;
}