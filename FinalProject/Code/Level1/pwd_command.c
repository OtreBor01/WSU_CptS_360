//
// Created by austinmm on 3/28/19.
//
#include "Level1.h"

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

int _pwd(char* pathname)
{
    MINODE* wd = _Running->cwd;
    char path[256] = "";
    int depth = r_pwd(0, path, wd);
    if(depth == 0){
        strcpy(path, "/");
    }
    puts(path);
}