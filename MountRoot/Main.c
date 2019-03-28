#include "Header.h"

void GetLine(char* line){
   puts("\n***** [ls|cd|pwd|quit] *****");
   printf("Command: ");
   fgets(line, 256, stdin);
   line[strlen(line)-1] = 0;
}

void SplitLine(char* line, char* path, char* cmd){
    sscanf(line, "%s %s", cmd, path);
    printf("cmd: '%s'\npathname: '%s'\n", cmd, path);
}

int OpenFile(char* disk){
	int fd = _DEV = open(disk, O_RDONLY | O_RDWR);
	if (fd < 0){
    printf("Error: Failed to open '%s'\n", disk);  
		exit(1);
	}
	return fd;
}

// Initialize data structures of LEVEL-1:
int init(void)
{
	//(1). 2 PROCs, P0 with uid=0, P1 with uid=1, all PROC.cwd = 0
  //(2). MINODE minode[64]; all with refCount=0
  //(3). MINODE *root = 0;
  MINODE *mip;
  printf("init()\n");
  for (int i=0; i<NMINODE; i++){
    mip = &minode[i];
    mip->dev = mip->ino = 0;
    mip->refCount = 0;
    mip->mounted = 0;
    mip->mptr = 0;
  }
  PROC *p;
  for (int i=0; i<NPROC; i++){
    p = &proc[i];
    p->pid = i;
    p->uid = 0;
    p->cwd = 0;
    p->status = FREE;
    for (int j=0; j<NFD; j++){
      p->fd[j] = 0;
  	}
  }
}

//mount root file system, establish / and CWDs: load root INODE and set root pointer to it
int mount_root(int dev)
{  
  printf("mount_root()\n");
  root = iget(dev, 2);
}

int init_proc(int dev){
	printf("Root Ref-Count = %d\n", root->refCount);
  puts("Creating P0 as Running Process");
  running = &proc[0];
  running->status = READY;
  running->cwd = iget(dev, 2); 
  printf("Root Ref-Count = %d\n", root->refCount);
}

int main(int argc, char *argv[ ])
{
  char *disk = "mydisk"; //Deafult Name
  int ino;
  char buf[BLKSIZE];
  if (argc > 1){
    disk = argv[1];
  }
  
  //Open Disk
  puts("Checking EXT2 FS ....");
	int fd = OpenFile(disk);
	//Get Super Block
	int sp_offset = 1; //Read super block at 1024
  get_block(fd, sp_offset, buf);
  sp = (SUPER*)buf;
  
	//Access/Check Super Block Information
  if (sp->s_magic != 0xEF53) //verify it's an ext2 file system
  {
      printf("Error: Magic = %x is Not an EXT2 Filesystem\n", sp->s_magic);
      exit(1);
  } 
  else{
    printf("Successfully Opened '%s'\n", disk);
  }    
  int ninodes = sp->s_inodes_count;
  int nblocks = sp->s_blocks_count;
 
	//Get Group Descripter
	int gp_offset = 2; //Read Group Descripter at 2048
  get_block(fd, gp_offset, buf); 
  gp = (GD*)buf;
	//Access Group Descripter Information
  int bmap = gp->bg_block_bitmap;
  int imap = gp->bg_inode_bitmap;
  inode_start = gp->bg_inode_table;
  printf("bmp = %d | imap = %d | inode_start = %d\n", bmap, imap, inode_start);
	
	//Initalize Essential Global Variables
  init();  
  mount_root(fd);
  init_proc(fd); 

  while(1)
  {
  	//Set string variables
  	char line[256]= "", cmd[32] = "", pathname[256] = "";
  	pathname[0] = 0;
    cmd[0] = 0;
    
    //Get User Input
  	GetLine(line);
    if (line[0] == 0){ continue; } //Invalid Input
    SplitLine(line, pathname, cmd);
    
		//Pause();
		//Execute the Command
    if (!strcmp(cmd, "ls")){
       ListFile(fd, pathname);
    }
    else if (!strcmp(cmd, "cd")){
       ChangeDir(fd, pathname);
    }
    else if (!strcmp(cmd, "pwd")){
       Pwd(fd, running->cwd);
    }
    else if (!strcmp(cmd, "quit")){
       Quit();
    }
  }
}
