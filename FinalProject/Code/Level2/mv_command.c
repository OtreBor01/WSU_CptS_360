//
// Created by austinmm on 4/17/19.
//

#include "Level2.h"

int _mv(char*pathname)
{
    //1. ***** Seperates Paths into Source and Dest *****
    char source[125] = "", dest[125] = "";
    int validPaths = seperate_path(pathname, source, dest);
    if(validPaths == -1){
        print_notice("mv: unable to locate source and dest path");
        return -1;
    }

    //2. ***** Get ino values for the base and dir files specified by source and dest paths
    //get inode numbers of files specified to move to and from
    int src_dev =_Running->cwd->dev;
    int dest_dev = _Running->cwd->dev;
    int src_ino = getino(source,&src_dev);
    int dest_ino = getino(dest,&dest);

    //get inode numbers of parent directories of files specified to move to and from..
    // checks if the parent directories are the root dir as well in the ternary operator
    char* base = get_parent_path(source);
    int psrc_dev = _Running->cwd->dev;
    int p_src_ino = getino(base, &psrc_dev);
    base = get_parent_path(dest);
    int pdest_dev = _Running->cwd->dev;
    int p_dest_ino = getino(base, &pdest_dev);

    //ensures that both the dir and base files exist according to the source pathname provided
    if(src_ino == 0 || p_src_ino == 0){
        print_notice("mv: unable to locate file operand");
        return -1;
    }

    //3. ***** Gets MINODEs for all the valid inode numbers obtained above *****
    int dev = _Running->cwd->dev;
    //old parent dir
    MINODE* p_src_mip = iget(psrc_dev, p_src_ino);
    //old file
    MINODE* src_mip = iget(src_dev, src_ino);
    //new parent dir
    MINODE* p_dest_mip = iget(pdest_dev, p_dest_ino);
    //old file
    MINODE* dest_mip = (dest_ino == 0)? NULL: iget(dest_dev, dest_ino);
    //if unable to locate parent dir of destination or destination is not a valid directory
    if(p_dest_ino == 0 || p_dest_mip == NULL || !S_ISDIR(p_dest_mip->INODE.i_mode)){
        print_notice("mv: unable to locate destination file operand");
        return -1;
    }
    //if the original file is a directory and thus cannot replace or be moved into an existing file that isn't a directory itself
    if(S_ISDIR(src_mip->INODE.i_mode)
       && dest_mip != NULL
       && !S_ISDIR(dest_mip->INODE.i_mode)
      )
    {
        print_notice("mv: cannot overwrite non-directory with directory");
        return -1;
    }
    char* dir = get_dest_path(source);
    //removes the original MINODE from its parent directory
    int fileType = mode_to_filetype(src_mip->INODE.i_mode);
    int isDir = (fileType == 2)? 1 : 0;
    if(remove_name(p_src_mip, dir, isDir) == 0) {
        print_notice("mv: unable to locate and remove file operand from parent directory");
        return -1;
    }

    //4. ***** Executes the actual file movements *****
    //if the destination is a existing directory
    if(dest_mip != NULL && S_ISDIR(dest_mip->INODE.i_mode)){
        return mv_into_dir(src_mip, p_dest_mip, dest_mip, dir);
    }
    //if the original file is not a directory
    dir = get_dest_path(dest);
    return replace_file(src_mip, p_dest_mip, dest_mip, dir);
}