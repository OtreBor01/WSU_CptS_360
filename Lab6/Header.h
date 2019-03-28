/*************************************************************************
	This C program, showblock, which displays the disk blocks of a file 
 	in an EXT2 file system. The program runs as follows
       showblock    DEVICE    PATHNAME
       ---------  ---------  -----------
	   showblock  diskimage  /a/b/c/d  (diskimage contains an EXT2 FS)

 	It locates the file named PATHNAME and prints the disk blocks (direct, 
 	indirect, double-indirect) of the file.
**************************************************************************/ 
#ifndef HEADER_H
#define HEADER_H

//***** Libraries *****
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ext2fs/ext2_fs.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>

//***** Custom Defined Types *****
typedef struct ext2_group_desc GD;
typedef struct ext2_super_block SUPER;
typedef struct ext2_inode INODE;
typedef struct ext2_dir_entry_2 DIR;

//***** Defined Constants *****
#define BLKSIZE 1024
#define PATHS 10
#define BASE_OFFSET 1024  /* location of the super-block in the first group */
#define BLOCK_OFFSET(block) (BASE_OFFSET + (block-1)*BLKSIZE)

//***** Global Variables *****
int _FD; //File Descripter
INODE* ROOT;

//***** Function Prototypes *****
int OpenFile(char*);
SUPER* GetSuperBlock(void);
GD* GetGroupDesc(int blk);
INODE* GetRootINode(int);
INODE* GetINode(int,GD*);
int GetBlock(int,char*);
void ShowDir(INODE*);
void ShowReg(INODE*);
void ShowIndirect(INODE*);
int TokenizePath(char*,char**);
void SearchTokens(INODE*, GD*,char**,int);
int SearchDir(INODE*,char*);
void Pause(void);
#endif

/***************************** SOURCE CODE *****************************
struct ext2_super_block {
	__u32	s_inodes_count;			//Inodes count
	__u32	s_blocks_count;			//Blocks count
	...
	__u32	s_free_blocks_count;	//Free blocks count
	__u32	s_free_inodes_count;	//Free inodes count
	__u32	s_first_data_block;		//First Data Block
	__u32	s_log_block_size;		//Block size
	...
	__u32	s_blocks_per_group;		//Number of Blocks per group
	...	
	__u16	s_magic;		 		//Magic signature
	...
}
########################################################################
struct ext2_group_desc
{
	__u32	bg_block_bitmap;		// Blocks bitmap block
	__u32	bg_inode_bitmap;		// Inodes bitmap block
	__u32	bg_inode_table;			// Inodes table block
	__u16	bg_free_blocks_count;	// Free blocks count
	__u16	bg_free_inodes_count;	// Free inodes count
	__u16	bg_used_dirs_count;		// Directories count
	__u16	bg_pad;
	__u32	bg_reserved[3];
}
########################################################################
struct ext2_dir_entry_2 {
	u32 inode; 						//inode number; count from 1, NOT 0
	u16 rec_len;					//this entry’s length in bytes
	u8 name_len;					//this entry’s length in bytes
	u8 file_type;					//name length in bytes not used
	char name[EXT2_NAME_LEN];		//name: 1-255 chars, no ending NULL
};
########################################################################
struct ext2_inode {
        __u16   i_mode;         	// File type and access rights 
        __u16   i_uid;          	// Low 16 bits of Owner Uid 
        __u32   i_size;         	// Size in bytes 
        __u32   i_atime;        	// Access time 
        __u32   i_ctime;        	// Creation time 
        __u32   i_mtime;        	// Modification time 
        __u32   i_dtime;        	// Deletion Time 
        __u16   i_gid;          	// Low 16 bits of Group Id 
        __u16   i_links_count;  	// Links count 
        __u32   i_blocks;       	// Blocks count 
        __u32   i_flags;        	// File flags 
	...
	__u32   i_block[EXT2_N_BLOCKS];	// Pointers to blocks
	...
};
########################################################################
  Type				  Macro
---------------------------------
Regular file		S_ISREG(m)
Directory			S_ISDIR(m)
Character Device  	S_ISCHR(m)
Block Device		S_ISBLK(m)
Fifo				S_ISIFO(m)
Socket				S_ISSOCK(m)
Symbolic Link		S_ISLNK(m)
***********************************************************************/
