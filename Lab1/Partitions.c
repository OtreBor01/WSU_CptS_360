#include <stdio.h>
#include <string.h>
#include <fcntl.h> 
//***** Custom Structures & Data Types *****
typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;

typedef struct partition {
	u8  drive;             /* drive number FD=0, HD=0x80, etc. */
	u8  head;             /* starting head */
	u8  sector;           /* starting sector */
	u8  cylinder;         /* starting cylinder */
	u8  sys_type;         /* partition type: NTFS, LINUX, etc. */
	u8  end_head;         /* end head */
	u8  end_sector;       /* end sector */
	u8  end_cylinder;     /* end cylinder */
	u32 start_sector;     /* starting sector counting from 0 */
	u32 nr_sectors;       /* number of of sectors in partition */
}Partition;

//***** Function Declarations *****
void LoadPartitions(Partition *p, int pCount);
void LoadExtendedPartitions(Partition *p, const u32 end, int pCount);
void getExtendedPartition(Partition **p, u32 n);
void printPartitionDetails(Partition * p, u32 end, int device);
int readDisk(char *diskName);

//***** DEfined Constants *****
#define BUFSIZE 512

//***** Global Variarbles *****
int INFILE = 0; 
char BUF[BUFSIZE];

int main(int argc, char *argv[], char *env[]){
	printf("%s\t   %s   %s %s  %s\n",
		"Device", "Boot Start", "End", "Sectors", "Id");
	INFILE = readDisk("vdisk"); //check that infile != -1
	Partition *p = (Partition *)&BUF[0x1BE];
	LoadPartitions(p, 1);	
	return 0;
}

void LoadPartitions(Partition *p, int pCount){
	if(p->start_sector != 0){
		u32 type = p->sys_type; //Partition's Type
		u32 start = p->start_sector; //Partition's Boot Start
		u32 end = p->nr_sectors + start - 1; //Partition's End
		printPartitionDetails(p, end, pCount); //Displays partition's details
		pCount++;
		if(type == 5){
			Partition *p_extend;
			getExtendedPartition(&p_extend, start);
			LoadExtendedPartitions(p_extend, end, pCount);
		}
		LoadPartitions(++p, pCount);		
	}
}

void LoadExtendedPartitions(Partition *p, const u32 end, int pCount){
	u32 currEnd = p->nr_sectors + p->start_sector - 1; //Current Extended Partition's End
	if(currEnd <= end){
		u32 type = p->sys_type; //Partition's Type
		u32 start = p->start_sector; //Partition's Boot Start
		if(type == 5){ //Extended Partition is an Extended partition itsself
			LoadPartitions(p, pCount);
		}
		else{
			printPartitionDetails(p, currEnd, pCount); //Displays partition's details
		}
		Partition *p_extend;
		getExtendedPartition(&p_extend, currEnd+1);
		LoadExtendedPartitions(p_extend, end, ++pCount);
	}
}
 
void getExtendedPartition(Partition **p, u32 n){
	lseek(INFILE, (long)(n*BUFSIZE), SEEK_SET); // lseek to current Partition's begin
	read(INFILE, BUF, BUFSIZE); // read local MBR
	*p = (Partition *)&BUF[0x1BE]; // access local ptable
	(*p)->start_sector += n;	
}

void printPartitionDetails(Partition * p, u32 end, int device){
	printf("vdisk%d\t   ", device); //Device
	printf("%10u  ", p->start_sector); //Boot Start
	printf("%4u ", end); //End
	printf("%7u  ", p->nr_sectors); //Sectors
	printf("%2x\n", p->sys_type); //Id
}

int readDisk(char *diskName){
	int infile = open(diskName, O_RDONLY); // open disk file for READ
	if(infile == -1){
		printf("Failed to open '%s' to read from...\n", diskName);
	}
	else{
		read(infile, BUF, BUFSIZE); // read MBR into buf[512]
	}
	return infile;
}
