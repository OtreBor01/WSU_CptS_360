#ifndef TYPE_H
#define TYPE_H

//***** Defined Constants *****

// Proc status
#define PROC_FREE    0
#define PROC_BUSY    1

// file system table sizes
#define NUM_MINODE     100
#define NUM_MTABLE     10
#define NUM_PROC       2
#define NUM_FD         10
#define NUM_OFT        40
#define BLKSIZE        1024

// Block number of EXT2 FS on FD
#define SUPERBLOCK      1
#define GDBLOCK         2
#define ROOT_INODE      2

//i_mode vlaues of EXT2 Reg and Dir
#define EXT2_S_IFREG	0x8000	/* regular file */
#define EXT2_S_IFDIR	0x4000	/* directory */

//***** Custom Defined Types *****
typedef unsigned char       u8;
typedef unsigned short      u16;
typedef unsigned int        u32;

typedef struct ext2_super_block     SUPER;
typedef struct ext2_group_desc      GD;
typedef struct ext2_inode           INODE;
typedef struct ext2_dir_entry_2     DIR;

// In-memory inodes structure
typedef struct minode{
  INODE             INODE; // disk inode
  int               dev, ino;
  int               refCount; // use count
  int               dirty; // modified flag
  int               mounted; // mounted flag
  struct mntable*   mptr; // mount table pointer
}MINODE;

// Open File Table
typedef struct oft{
  int          mode;
  int          refCount;
  MINODE*      minodePtr;;
  int          offset;
}OFT;

// PROC structure
typedef struct proc{
  struct proc *next;
  int          pid;
  int          uid;
  int          gid;
  int          ppid;
  int          status;
  MINODE*      cwd;
  OFT*         fd[NUM_FD];
}PROC;

// Mount Table structure
typedef struct mtable{
  int          dev; // device number; 0 for FREE
  int          ninodes; // from superblock
  int          nblocks;
  int          free_blocks; // from superblock and GD
  int          free_inodes;
  int          bmap; // from group descriptor
  int          imap;
  int          iblock; // inodes start block
  MINODE*      mntDirPtr; // mount point DIR pointer
  char         devName[64]; //device name
  char         mntName[64]; // mount point DIR name
}MTABLE;

//***** Global Variables *****
SUPER*      _Super;
GD*         _GroupDec;
MINODE*      _Root;
MINODE      _MINodes[NUM_MINODE]; //in memory INODEs
MTABLE      _MTables[NUM_MTABLE]; //mount tables
OFT         _Ofts[NUM_OFT]; //Opened file instance
int         _OpenOFT;
PROC        _Procs[NUM_PROC]; //PROC structures
PROC*       _Running; //current executing PROC
int         _IStartBlock;
int         _NumberOfBlocks;

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
