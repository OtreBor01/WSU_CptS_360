//
// Created by austinmm on 4/17/19.
//

#include "Level2.h"

//The original file is a directory so it can only be moved into another directory with its original name or...
// under a new one specified by the new path
int mv_into(MINODE* o_mip, char* name, MINODE* n_mip)
{
    return enter_name(n_mip, name, o_mip->ino, o_mip->INODE.i_mode);
}

int mv_replace(MINODE* o_mip, MINODE* p_n_mip, MINODE* n_mip, char* name)
{
    //if there exist a file to be replaced in the destination then...
    // remove the existing file from the destination parent directory
    if(n_mip != NULL) {
        remove_name(p_n_mip, name);
    }
    enter_name(p_n_mip, name, o_mip->ino, o_mip->INODE.i_mode);
    return -1;
}

int seperate_path(char* path, char* source, char* dest)
{
    sscanf(path, "%s %s", source, dest);
    if(!strcmp(source, "")) {
        print_notice("mv: missing file operand");
        return -1;
    }
    else if (!strcmp(dest, "")) {
        print_notice("mv: missing destination file operand");
        return -1;
    }
    return 0;
}


int _mv(char*pathname)
{
    //1. ***** Seperates Paths into Source and Dest *****
    char source[125] = "", dest[125] = "";
    int validPaths = seperate_path(pathname, source, dest);
    if(validPaths == -1){ return -1; }

    //2. ***** Get ino values for the base and dir files specified by source and dest paths
    //get inode numbers of files specified to move to and from
    int src_ino = getino(source);
    int dest_ino = getino(dest);

    //get inode numbers of parent directories of files specified to move to and from..
    // checks if the parent directories are the root dir as well in the ternary operator
    char* base = strchr(source, '/')?  basename(source) : "";
    int p_src_ino = getino(base);
    base = strchr(dest, '/')? basename(dest) : "";
    int p_dest_ino = getino(base);

    //ensures that both the dir and base files exist according to the source pathname provided
    if(src_ino == 0 || p_src_ino == 0){
        print_notice("mv: unable to locate file operand");
        return -1;
    }

    //3. ***** Gets MINODEs for all the valid inode numbers obtained above *****
    int dev = _Running->cwd->dev;
    //old parent dir
    MINODE* p_src_mip = iget(dev, p_src_ino);
    //old file
    MINODE* src_mip = iget(dev, src_ino);
    //new parent dir
    MINODE* p_dest_mip = iget(dev, p_dest_ino);
    //old file
    MINODE* dest_mip = (dest_ino == 0)? NULL: iget(dev, dest_ino);
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
    char* dir = strchr(source, '/')?  dirname(source) : source;
    //removes the original MINODE from its parent directory
    if(remove_name(p_src_mip, dir) == 0) {
        print_notice("mv: unable to locate and remove file operand from parent directory");
        return -1;
    }

    //4. ***** Executes the actual file movements *****
    //if the destination is a existing directory
    if(dest_mip != NULL && S_ISDIR(dest_mip->INODE.i_mode)){
        dir = strchr(source, '/')?  dirname(source) : source;
        return mv_into(src_mip, dir, dest_mip);
    }
    //if the original file is not a directory
    dir = strchr(dest, '/')?  dirname(dest) : dest;
    return mv_replace(src_mip, p_dest_mip, dest_mip, dir);
}