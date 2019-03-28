/************* cd_ls_pwd.c file **************/
#include "Header.h"

#define OWNER  000700
#define GROUP  000070
#define OTHER  000007

int ChangeDir(int dev, char* path)
{
	MINODE *mip = running->cwd;
	char buf[BLKSIZE];
	if(!strcmp(path, "")) {
		return 0;
	}
	else{
		//search for path to print
		int ino = getino(mip,path);
		if(ino == 0) { return -1; }
		mip = iget(dev, ino);
		if(S_ISDIR(mip->INODE.i_mode))
		{
			running->cwd = mip;
		}
		else if(S_ISREG(mip->INODE.i_mode)){
			printf("Cannot cd into a file\n");
		}
		iput(mip);
	}
	return 0;
}


int ListFile(int dev, char* path)
{
	MINODE *mip = running->cwd;
	char buf[BLKSIZE];
	if(!strcmp(path, "")) { //ls cwd
		ls_dir(mip);
	}
	else{
		//search for path to print
		int ino = getino(mip,path);
		if(ino == 0) { return -1; }
		mip = iget(dev, ino);
		if(S_ISDIR(mip->INODE.i_mode))
		{
			ls_dir(mip);
		}
		else if(S_ISREG(mip->INODE.i_mode)){
			ls_reg(mip, path);
		}
		iput(mip);
	}
	return 0;
}


int Pwd(int dev, MINODE* wd)
{
	char path[256] = "";
	R_Pwd(path, dev, wd);
	puts(path);
}


int R_Pwd(char *path, int dev, MINODE* wd)
{
     if (wd == root) { return 0; }
     int ino = search(wd, "..");
     MINODE* mip = iget(dev, ino);
     char* name = GetName(&mip->INODE, wd->ino);
     if(name == NULL){ return -1; }
     char temp[256] = "";
     int isEmpty = !strcmp(path, "");
     strcpy(temp, path);
	 strcpy(path, "/");
	 strcat(path, name);
	 if(!isEmpty){
     	strcat(path, temp);
     }
     R_Pwd(path, dev, mip);

}

char* GetName(INODE* ip, int ino){
	for(int i = 0; i < ip->i_blocks; i++){
		char buf[BLKSIZE];
		int blk = ip->i_block[i];
		get_block(_DEV, blk, buf);
		DIRE* dp = (DIRE*)buf;
		char *cp = buf;
		while(cp < (buf + BLKSIZE)){
			if(dp->inode == ino){
				dp->name[dp->name_len] = 0; // ensure NULL at end
				return dp->name;
			}
			cp += dp->rec_len; // advance cp by rec_len
			dp = (DIRE*)cp; // pull dp to next entry
		}	
	}
	return NULL;
}

int Quit(void)
{
  MINODE *mip;
  for (int i=0; i<NMINODE; i++){
    mip = &minode[i];
    if (mip->refCount > 0){
      iput(mip);
    }
  }
  exit(0);
}
