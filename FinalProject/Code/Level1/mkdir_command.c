//
// Created by noah on 4/2/19.
//

#include "Level1.h"

int _mkdir(char* pathname){
    //Grab dir name and path name
    //Grab ino to check if dir exist and is a DIR type
    int parent_dev = _Running->cwd->dev;
    char* path = get_parent_path(pathname);
    int pino = getino(path, &parent_dev);
    if(pino == 0){
        print_notice("mkdir: could not locate parent directory");
        return -1;
    }
    MINODE * pmip = iget(parent_dev, pino);
    if(!S_ISDIR(pmip->INODE.i_mode)){
        print_notice("mkdir: could not locate parent directory");
        return -1;
    }
    if(search(pmip, path, &parent_dev) != 0){
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
