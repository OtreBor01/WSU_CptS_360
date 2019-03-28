#include "Header.h"

int OpenFile(char* device){
	int fd = _FD = open(device, O_RDONLY | O_RDWR);
	if (fd < 0){
		printf("Open '%s' Failed\n", device); 
		exit(1);
	}
	return fd;
}
//***** Function Declarations *****
SUPER* GetSuperBlock(void)
{
	char buf[BLKSIZE];
	int address = GetBlock(1, buf); //read super-block address
	SUPER *sp = (SUPER*)buf; //converts address to struct ext2_super_block
	// check EXT2 FS magic number:
	if (sp->s_magic != EXT2_SUPER_MAGIC){ //bad file system: 0xEF53
		puts("(1). Verify it's an ext2 file system: FAILURE");
		exit(2);
	}
	return sp;
}

GD* GetGroupDesc(int blk)
{
	char buf[BLKSIZE];
	int address = GetBlock(2, buf); //read super-block address
	GD *gd = (GD*)buf; //converts address to struct ext2_super_block
	return gd;
}

INODE* GetRootINode(int blk)
{
	char buf[BLKSIZE];
	int address = GetBlock(blk, buf); //read super-block address
	INODE *root = (INODE*)buf; //converts address to struct ext2_super_block
	return root;
}

INODE* GetINode(int inode_no, GD* groupDesc)
{
	char buf[BLKSIZE];
	int blk = BLOCK_OFFSET(groupDesc->bg_inode_table)+(inode_no-1);
	int address = GetBlock(blk, buf); //read super-block address
	SUPER *sp = (SUPER*)buf; //converts address to struct ext2_super_block
	INODE* inode = (INODE*)buf;
	return inode;
}

// get_block() reads a disk BLOCK into a char buf[BLKSIZE].
int GetBlock(int blk, char* buf)
{
	long size = blk*BLKSIZE;
  	lseek(_FD, size, SEEK_SET);
  	return read(_FD, buf, BLKSIZE);
}

//prints the entries of a Directory INODE: INODE *ip -> INODE of a DIRectory:
void ShowDir(INODE* ip)
{
	puts("| i_number | rec_len | name_len | name");
	puts("|-------------------------------------");
	for (int i=0; i < ip->i_blocks; i++){  // assume DIR at most 12 direct blocks
		int blk = ip->i_block[i];
		char buf[BLKSIZE];
		int address = GetBlock(blk, buf); // get data block into buf[ ]
		DIR *dp = (DIR*)buf; //// as dir_entry
		char *cp = buf;
		char temp[256];
		while(cp < (buf + BLKSIZE)){
			strncpy(temp, dp->name, dp->name_len); // make name a string
			temp[dp->name_len] = 0; // ensure NULL at end
			printf("| %8d | %7d | %8d | %s\n", dp->inode, dp->rec_len, dp->name_len, temp);
			cp += dp->rec_len; // advance cp by rec_len
			dp = (DIR*)cp; // pull dp to next entry
		}
	}
}

void ShowReg(INODE* ip){
	printf("Size: %d\n", ip->i_size);
	int blk = 0;
	char buf[BLKSIZE];
	for(int i = 0; i < EXT2_N_BLOCKS; i++){
		if (i < EXT2_NDIR_BLOCKS)// direct blocks
		{
			blk = ip->i_block[i];
			printf("i_block[%d]: %d\n", i, blk);
		}
		else{
			if (i == EXT2_IND_BLOCK) //single indirect block
			{
				puts("----------- INDIRECT BLOCKS ---------------");
				blk = ip->i_block[EXT2_IND_BLOCK];
				int address =  GetBlock(blk, buf); // get data block into buf[ ]
				ip = (INODE*)buf-10;
				ShowIndirect(ip);
			}
			else if (i == EXT2_DIND_BLOCK) //double indirect block
			{
				//puts("----------- DOUBLE INDIRECT BLOCKS ---------------");
				//ShowIndirect(ip);
			}
			blk = buf[i-12]; //// as dir_entry
			printf("%4d ", blk);
		}
	}
	Pause();
}

void ShowIndirect(INODE* ip){
	Pause();
	for(int i = 1; i < ip->i_blocks; i++){
		int size = ip->i_block[i-1];
		if(i % 10 == 0){
			printf("| %4d |\n", size);
		}
		else{
			printf("| %4d ", size);
		}
	}
	puts("");
}

//Searches the DIrectory's data blocks for a name string: return its inode number if found; return 0 if not.
int SearchDir(INODE *ip, char *name){
	printf("Search For '%s' in %x\n", name, (int)ip);
	//printf("i=0  i_block[0]=33");
	puts("| i_number | rec_len | name_len | name");
	puts("|-------------------------------------");
	for(int i = 0; i < ip->i_blocks; i++){
		int blk = ip->i_block[i];
		char buf[BLKSIZE];
		int address = GetBlock(blk, buf); // get data block into buf[ ]
		DIR *dp = (DIR*)buf; //// as dir_entry
		char *cp = buf;
		char temp[256];
		while(cp < (buf + BLKSIZE)){
			strncpy(temp, dp->name, dp->name_len); // make name a string
			temp[dp->name_len] = 0; // ensure NULL at end
			printf("| %8d | %7d | %8d | %s\n", dp->inode, dp->rec_len, dp->name_len, temp);
			if(!strcmp(name, temp)){
				printf("Found '%s': inode = %d\n", name, dp->inode);
				return dp->inode;
			}
			cp += dp->rec_len; // advance cp by rec_len
			dp = (DIR*)cp; // pull dp to next entry
		}	
	}
	return 0;
}

void SearchTokens(INODE* ip, GD* groupDesc, char **tokens, int size){
 	char ibuf[BLKSIZE];
	int imapblk = groupDesc->bg_inode_bitmap; // get imap block number
	int inodes_start = groupDesc->bg_inode_table;
	for(int i = 0; i < size ; i++){
		char* name = tokens[i];
		int ino = SearchDir(ip, name);
		//(7). When the above for loop ends, ip MUST point at the INODE of pathname.
		// Mailman's algorithm: Convert (dev, ino) to INODE pointer
		int blk = (ino - 1) / 8 + inodes_start; 
		int offset = (ino - 1) % 8;        
		int addresss = GetBlock(blk, ibuf);
		blk = (ino - 1) / 8 + inodes_start;
		offset = (ino - 1) % 8;
		ip = (INODE*)ibuf + offset;   // ip -> new INODE
		//ip = GetINode(ino, groupDesc);
		if (ino == 0){
			printf("Error: Failure to Find '%s'\n", name); 
			exit(1);
		}
		//(8). Extract information from ip-> as required:
		if(S_ISREG(ip->i_mode)){
			ShowReg(ip);
			break;
		}
	}
}

int TokenizePath(char* path, char **tokens){
	printf("Tokenize: '%s'\n", path);
	char *token = strtok(path, "/");
	int i = 0;
	for(; token; i++){
		if(strlen(token) != 0 && i < PATHS){ //not an empty string
			tokens[i] = token;
			if(i == 0){
				printf("Tokens: %s ", token);
			}
			else { printf("| %s ", token); }
		}
		token = strtok(NULL, "/");
	}
	puts("");
	return i+1;
}

	
void Pause(void){
 	puts("\n*** Press Any To Continue ***");
	getchar();
}
