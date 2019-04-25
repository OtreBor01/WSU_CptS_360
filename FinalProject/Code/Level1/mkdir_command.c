//
// Created by noah on 4/2/19.
//

#include "Level1.h"

int _mkdir(char* pathname){
    int pino;
    MINODE * pmip;
    //Grab dir name and path name
    //Grab ino to check if dir exist and is a DIR type
    char* path = get_parent_path(pathname);
    pino = getino(path);
    if(pino == 0){
        print_notice("mkdir: could not locate parent directory");
        return -1;
    }
    pmip = iget(_Running->cwd->dev, pino);
    if(!S_ISDIR(pmip->INODE.i_mode)){
        print_notice("mkdir: could not locate parent directory");
        return -1;
    }
    if(search(pmip,path) != 0){
        print_notice("mkdir: directory with that name already exist");
        return -1;
    }
    path = get_dest_path(pathname);
    kmkdir(pmip,path);
    pmip->INODE.i_links_count = 1;
    pmip->dirty = 1;
    iput(pmip);
    return 0;
}
