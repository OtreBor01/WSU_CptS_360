#include "Header.h"

int main(int argc, char *argv[], char*env[]){
 	char* device = "diskimage"; //default value
 	if (argc > 1){
 		device = argv[1];
 	}
 	char* path = argv[2];
 	if (argc > 2){
 		path = argv[2];
 	}
	
	//1.) Open diskimage for READ or RDWR: Use file descriptor dev as device number
 	int fd = OpenFile(device);
	
	//2.) Read in SUPER block (block #1) to verify diskimage is an EXT2 FS
	SUPER* super = GetSuperBlock();
	puts("(1). Verify it's an ext2 file system: OK");
	int block_size = 1024 << super->s_log_block_size; //calculate block size in bytes	
	//3.) Read in Group Descriptor 0 (in block #2) to get block number of bmap, imap, inodes_start;  Print their values.
		//In the blocks immediately following the super-block reside the list of block-group descriptors. This list contains a descriptor for each block group on the disk.
	puts("(2). Read group descriptor 0 to get bmap imap inodes_start");
	GD* groupDesc = GetGroupDesc(block_size), gd = *groupDesc;
	printf("bmap = %d | imap = %d | inodes_start = %d\n",  
			groupDesc->bg_block_bitmap, groupDesc->bg_inode_bitmap, groupDesc->bg_inode_table);
	
	//4.) Let INODE *ip -> root INODE in memory:
	int iblock = groupDesc->bg_inode_table;
	INODE* root = GetRootINode(iblock);
	++root; // point at #2 INODE
	INODE ip = *root;
	
	printf("Root: %x\n", (int)root);
	puts("*********** Root INODE *************");
	puts("(3). Show root DIR contents");
	printf("Root INODE Data Block = %d\n",root->i_block[0]);
	//(4).1: Print contents of the root DIRectory
	ShowDir(root);
	Pause();
	//(5). Tokenize pathname into components, e.g. /cs360/is/fun ==>
	char *tokens[PATHS] = {NULL};
	int size = TokenizePath(path, tokens);	//tokenize /Z/hugefile
	//(6). Start from the root inode #2
	SearchTokens(&ip, &gd, tokens, size);
	return 0;
}

/*

//calculate number of block groups on the disk
unsigned int group_count = 1 + (super->s_blocks_count-1) / super->s_blocks_per_group;
//calculate size of the group descriptor list in bytes
unsigned int descr_list_size = group_count * sizeof(GD);
int inodes_start = groupDesc->bg_inode_table;
*/
