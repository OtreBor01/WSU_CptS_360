/*********** util.c file ****************/
#include "Header.h"

int get_block(int dev, int blk, char *buf)
{
	 long offset = (long)blk*BLKSIZE;
   lseek(dev, offset, 0);
   return read(dev, buf, BLKSIZE);
}   

int put_block(int dev, int blk, char *buf)
{
	 long offset = (long)blk*BLKSIZE;
   lseek(dev, offset, 0);
   return write(dev, buf, BLKSIZE);
}   

int tokenize(char *pathname, char** name)
{
	char* token = strtok(pathname, "/");
	int i = 0;
	while(token){
		if(strcmp(token, "") ){
			name[i++] = token; 
		}
		token = strtok(NULL, "/");
	}
	return i;
}

// return minode pointer to loaded INODE
MINODE *iget(int dev, int ino)
{
  MINODE *mip;
  for (int i=0; i<NMINODE; i++){
    mip = &minode[i];
    if (mip->dev == dev && mip->ino == ino){
       mip->refCount++;
       printf("Found [%d %d] as MINODE[%d] in Core\n", dev, ino, i);
       return mip;
    }
  }
     
  for (int i=0; i<NMINODE; i++){
    mip = &minode[i];
    if (mip->refCount == 0){
       char buf[BLKSIZE];
       mip->refCount = 1;
       mip->dev = dev;
       mip->ino = ino;

       // get INODE of ino to buf    
       int blk  = (ino-1) / 8 + inode_start;
       int disp = (ino-1) % 8;

       get_block(dev, blk, buf);
       INODE *ip = (INODE *)buf + disp;
       // copy INODE to mp->INODE
       mip->INODE = *ip;
       return mip;
    }
  }   
  printf("PANIC: no more free minodes\n");
  return 0;
}

void iput(MINODE *mip)
{
	if (mip == 0){ return; }

	mip->refCount--;

	if (mip->refCount > 0){ return; }
	if (!mip->dirty) { return; }

	/* write back */
	printf("iput: dev = %d | ino = %d\n", mip->dev, mip->ino); 

	int block =  ((mip->ino - 1) / 8) + inode_start;
	int offset = (mip->ino - 1) % 8;

	/* first get the block containing this inode */
	char buf[BLKSIZE];
	get_block(mip->dev, block, buf);
	INODE *ip = (INODE *)buf + offset;
	*ip = mip->INODE;

	put_block(mip->dev, block, buf);

} 

int search(MINODE *mip, char *name)
{
	INODE* ip = &(mip->INODE);
	for(int i = 0; i < ip->i_blocks; i++){
		char buf[BLKSIZE];
		int blk = ip->i_block[i];
		get_block(_DEV, blk, buf);
		DIRE* dp = (DIRE*)buf;
		char *cp = buf;
		char temp[256];
		while(cp < (buf + BLKSIZE)){
			strncpy(temp, dp->name, dp->name_len); // make name a string
			temp[dp->name_len] = 0; // ensure NULL at end
			if(!strcmp(name, temp)){
				return dp->inode;
			}
			cp += dp->rec_len; // advance cp by rec_len
			dp = (DIRE*)cp; // pull dp to next entry
		}	
		
	}
}

int getino(MINODE *mip, char *pathname)
{
  printf("getino: pathname = '%s'\n", pathname);
  if (!strcmp(pathname, "/")){
      return 2;
  }
  else if (pathname[0] == '/'){
    mip = iget(_DEV, 2);
  }
  else{
    mip = iget(running->cwd->dev, running->cwd->ino);
  }
  
	char *name[64];  // assume at most 64 components in pathname
  int n = tokenize(pathname, name); // number of component strings
	int ino = 0;
  for (int i=0; i < n; i++){
      puts("===========================================");
      ino = search(mip, name[i]);
      if (ino==0){
         iput(mip);
         printf("Name '%s' Does Not Exist\n", name[i]);
         return 0;
      }
      iput(mip);
      mip = iget(_DEV, ino);
   }
   return ino;
}

