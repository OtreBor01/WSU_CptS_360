/*
The following shows a simple ls program which behaves like the ls –l command of Linux. The purpose
here is not to re-invent the wheel by writing yet another ls program. Rather, it is intended to show how
to use the various syscalls to display information of files under a directory. By studying the example
program code, the reader should also be able to figure out how Linux’s ls command is implemented.
*/

//***** Libraries *****
#include "Header.h"

//***** Global Variables *****

void mode_to_letters(mode_t mode, char *str)
{
	//dir
	if (S_ISDIR(mode)) { str[0] = 'd'; }
	//link
	else if (S_ISLNK(mode)) { str[0] = 'l'; }
	//reg, i.e. if (S_ISREG(mode))
	else { str[0] = '-'; }
    str[1] = (mode & S_IRUSR) ? 'r' : '-';
    str[2] = (mode & S_IWUSR) ? 'w' : '-';
    str[3] = (mode & S_IXUSR) ? 'x' : '-';
    str[4] = (mode & S_IRGRP) ? 'r' : '-';
    str[5] = (mode & S_IWGRP) ? 'w' : '-';
    str[6] = (mode & S_IXGRP) ? 'x' : '-';
    str[7] = (mode & S_IROTH) ? 'r' : '-';
    str[8] = (mode & S_IWOTH) ? 'w' : '-';
    str[9] = (mode & S_IXOTH) ? 'x' : '-';
    str[10] = '\0';
}

int ls_reg(MINODE *mip, char *name)
{
	INODE *ip = &mip->INODE;
	//information for each file
	u16 mode  = ip->i_mode;
    u16 links = ip->i_links_count;
    u16 uid = ip->i_uid;
    u16 gid = ip->i_gid;
    u32 size = ip->i_size;
    //Mode
    char modes[11] = "";
	memset(modes, 0,sizeof(modes));
	mode_to_letters(mode, modes);
	//Time
	time_t* t = (time_t*)&ip->i_mtime;
	char *time = ctime(t);
	time[strlen(time) - 1] = 0;	//remove \r from time

	//print the rest
	printf("%s %4hu %4hu %4hu %8u %26s %s\n", modes, links, gid, uid, size, time, name);
	return 0;
}


int ls_dir(MINODE *mip){
	INODE *ip = &mip->INODE;
	for(int i = 0; i < ip->i_blocks; i++){
		int blk = ip->i_block[i];
		if(blk == 0) { break; }
		printf("i_block[%d] = %d\n", i,  blk);
		char buf[BLKSIZE];
		int address = get_block(_DEV, blk, buf); // get data block into buf[ ]
		DIRE *dp = (DIRE*)buf; //// as dir_entry
		char *cp = buf;
		char temp[256];
		while(cp < (buf + BLKSIZE)){
			strncpy(temp, dp->name, dp->name_len); // make name a string
			temp[dp->name_len] = 0; // ensure NULL at end
			MINODE *temp_mip = iget(_DEV, dp->inode);
			if(temp_mip)
			{
				ls_reg(temp_mip, temp);
				iput(temp_mip);
			}
			cp += dp->rec_len; // advance cp by rec_len
			dp = (DIRE*)cp; // pull dp to next entry
		}	
	}
	return 0;
}
