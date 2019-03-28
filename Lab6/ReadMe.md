LAB Assignment \#6
==================

     ******************** DUE: Friday, 3-26-2019 **********************
           TURN IN A HARD COPY with outputs for the file /Z/hugefile 
         
           QUIZ : 3-28-2019 in class on LAB6 code
     ******************************************************************

     Write a C program, showblock, which displays the disk blocks of a file 
     in an EXT2 file system. The program runs as follows

           showblock    DEVICE    PATHNAME
           ---------  ---------  -----------
    e.g.   showblock  diskimage  /a/b/c/d  (diskimage contains an EXT2 FS)

     It locates the file named PATHNAME and prints the disk blocks (direct, 
     indirect, double-indirect) of the file.
    **************************************************************************/ 

                   HOW TO Traverse EXT2 File System Tree

    1. How to traverse the EXT2 FS tree:

       Given a device, diskimage, containing an ext2 FS, and a pathname, e.g.
                       /cs360/is/fun
       of a file, find the file.

       NOTE!!! To find a file amounts to finding its INODE. 
               From its inode, you have ALL the information of the file.
      
    2. ALGORITHM:

    (1). Open diskimage for READ or RDWR: Use file descriptor dev as device number.

              #include <fcntl.h>
              #define BLKSIZE 1024

              int dev = open("diskimage", O_RDONLY);   // OR  O_RDWR

              // get_block() reads a disk BLOCK into a char buf[BLKSIZE].
       
              int get_block(in dev, int blk, char *buf)
              {   
                  lseek(dev, blk*BLKSIZE, SEEK_SET);
                  return read(dev, buf, BLKSIZE);
              }
     
     
    (2). Read in SUPER block (block #1) to verify diskimage is an EXT2 FS

    (3). Read in Group Descriptor 0 (in block #2) to get block number of 
         bmap, imap, inodes_start;  Print their values.

    (3). Let INODE *ip -> root INODE in memory: HOW:
       
             read in inodes_start block into (global) char ibuf[BLKSIZE];
             ip = (INODE *)ibuf + 1;  // do NOT change ibuf[] while using ip

    (4). The following C code prints the entries of a DIRectory INODE:

         INODE *ip -> INODE of a DIRectory:

         show_dir(INODE *ip)
         {
            char sbuf[BLKSIZE], temp[256];
            DIR *dp;
            char *cp;
            int i;
     
            for (i=0; i < 12; i++){  // assume DIR at most 12 direct blocks
                if (ip->i_block[i] == 0)
                   break;
                // YOU SHOULD print i_block[i] number here
                get_block(dev, ip->i_block[i], sbuf);

                dp = (DIR *)sbuf;
                cp = sbuf;
     
                while(cp < sbuf + BLKSIZE){
                   strncpy(temp, dp->name, dp->name_len);
                   temp[dp->name_len] = 0;
                   printf("%4d %4d %4d %s\n", 
                     dp->inode, dp->rec_len, dp->name_len, temp);

                   cp += dp->rec_len;
                   dp = (DIR *)cp;
               }
            }
         }

    (4).1: Print contents of the root DIRectory

    (4).2: Modify show_dir() to wrtie a search() function

              int search(INODE *ip, char *name)
      
       which searches the DIrectory's data blocks for a name string; 
             return its inode number if found; return 0 if not.

    (5). Tokenize pathname into components, e.g. /cs360/is/fun ==>

         char *name[0]  name[1]  name[2];  with n = 3
                |        |        |
               "cs360"  "is"     "fun"

    (6). Start from the root inode #2

             int ino, blk, offset;
             INODE *ip->root inode;

             for (i=0; i < n; i++){
                 ino = search(ip, name[i]);
            
                 if (ino==0){
                    printf("can't find %s\n", name[i]); 
                    exit(1);
                 }
      
                 // Mailman's algorithm: Convert (dev, ino) to INODE pointer
                 blk    = (ino - 1) / 8 + inodes_start; 
                 offset = (ino - 1) % 8;        
                 get_block(dev, blk, ibuf[ ]);
                 ip = (INODE *)ibuf + offset;   // ip -> new INODE
             }
      
    (7). When the above for loop ends, ip MUST point at the INODE of pathname.

    (8). Extract information from ip-> as required:

           Print direct block numbers;
           Print indirect block numbers; 
           Print double indirect block numbers, if any

         Pat yourself on the back and say: Good Job!

    ===============================================================================
    SAMPLES SOLUTION in samples/LAB6/:

            diskimage: virtual disk with DIRs and FILES
            showDisk : a sh script to display diskimage contents
            lab6.bin : run as  lab6.bin diskimage PATHNAME 

                      REVIEW QUESTIONS:

    1. Given the INODE of a DIRectory, 
       Write C code to print all names under this DIR:


    2. Given the INODE of a file,
       Write C code to print all the INdirecto block numbers of this file:
