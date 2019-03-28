CS360 LAB ASSIGNMENT \#1
========================

                      CS360 LAB ASSIGNMENT #1
                      DUE & DEOM: Jan 18, 2019

    PART 1: Partition Table

    1. OBJECTIVES
       Partition table, fdisk, structures in C, open-read files

    2 Partition Table:

    A disk (floppy disk, hard disk, USB drive, SD cards, etc.) consists of 512-byte
    sectors, which are counted linearly as sector 0,1,2,....
     
    A disk is usually divided into several partitions. The partitions are recorded 
    in a partition table at the beginning (the 0th sector) of the disk, called the 
    Master Boot Record (MBR). Inside the MBR, the partition table begins at the
    byte offset 0x1BE. The Partitin Table contains 4 entries, each 16 bytes long, 
    defined in the following C structure.

    typedef unsigned char  u8;
    typedef unsigned short u16;
    typedef unsigned int   u32;

    struct partition {
        u8 drive;             /* drive number FD=0, HD=0x80, etc. */

        u8  head;             /* starting head */
        u8  sector;           /* starting sector */
        u8  cylinder;         /* starting cylinder */

        u8  sys_type;         /* partition type: NTFS, LINUX, etc. */

        u8  end_head;         /* end head */
        u8  end_sector;       /* end sector */
        u8  end_cylinder;     /* end cylinder */

        u32 start_sector;     /* starting sector counting from 0 */
        u32 nr_sectors;       /* number of of sectors in partition */
    };

    heads, sectors, cylinders are for old floppy disks. 
    Hard disks only use start_sector and nr_sectors.
     
    Each partition has a type, which indicates what kind of file system the 
    partition MAY contain. Consult Linux's fdisk to see the partition types.

    If a partition is EXTEND type (type=5), the partition's area may be further 
    divided into more partitions. The extended partitions forms a LINK-LIST as the 
    following diagram shows. 
    ------------------------------------------------------------------------------
    Assume P4 is EXT type:

    P4's beginSector = localMBR
                         P5's beginSector# relative to beginSector
                         P6's MBR's sector# = localMBR
                             (r.e. to P4)       P6's beginSector# 
                                                P7's MBR r.e. to P4 --> etc.

    The first sector of each extended partition is a localMBR. Each localMBR has a
    partition table which contains only 2 entries. The first entry defines the start
    sector and size of the extended partition. The second entry points to the next
    localMBR. All the localMBR's sector numbers are relative to P4's start sector.
    As usual, the link list ends with a 0.
    -------------------------------------------------------------------------------

    Since use fdisk on hard disks is risky, we shall use a VIRTUAL disk for this
    assignment. A virtual disk is just a file but its contents are exactly the same
    as a REAL disk. Download the file ~cs360/samples/LAB1/vdisk to YOUR Linux. 
    Then, run       fdisk vdisk 
                     'p' : to see the partition table
                     'q' : to quit fdisk

                          REQUIREMENTS

    Write a C progrom to display the partition table of the vdisk in
     
    (1). Linux fdisk 'p' output form for the first 4 partitions (%40),
    (2). including ALL the extend partitions (%60) <== YOU BETTER DO THIS !!!!.

    Turn in a printed HARD COPY containing your work. 


    =========================== Part 2 ======================================

    PART 2: myprintf Function

    Given: putchar(char c) of Linux, which prints a char.

    2-1. Write YOUR own prints(char *s) fucntion to print a string.

    Given: The following printu() function prints an unsigned integer.

    typedef unsigned int u32;

    char *ctable = "0123456789ABCDEF";
    int  BASE = 10; 

    int rpu(u32 x)
    {  
        char c;
        if (x){
           c = ctable[x % BASE];
           rpu(x / BASE);
           putchar(c);
        }
    }

    int printu(u32 x)
    {
       (x==0)? putchar('0') : rpu(x);
       putchar(' ');
    }

    2-2. Write YOUR ONW fucntions 

     int  printd(int x) which prints an integer (x may be negative!!!)
     int  printx(u32 x) which prints x in HEX   (start with 0x )
     int  printo(u32 x) which prints x in Octal (start with 0  )

    3. REQUIREMENTS:
    ====================================================================
       Write YOUR own myprintf(char *fmt, ...) function to print 
             char                      by %c
             string                    by %s
             unsigned integer          by %u
             integer                   by %d
             unsigned integer in OCT   by %o
             unsigned integer in HEX   by %x
       Ignore field width and precision, just print the items as specified.

    2-2. In the int main(int argc, char *argv[ ], char *env[ ]) function, 
         use YOUR myprintf() to print
                  argc value
                  argv strings
                  env  strings
     
        myprintf("cha=%c string=%s      dec=%d hex=%x oct=%o neg=%d\n", 
               'A', "this is a test", 100,    100,   100,  -100);

    /********************** HELP INFO **************************/
    NOTE: This assignment is for 32-bit GCC, which passes parameters on stack.
          Use  
                   gcc -m32 t.c

          to compile your C source files.

    int myprintf(char *fmt, ...)      // C compiler requires the 3 DOTs
    {
       Assume the call is myprintf(fmt, a,b,c,d);
       Upon entry, the following diagram shows the stack contents.
     
                    char *cp -> "...%c ..%s ..%u .. %d\n"
       HIGH               |                                              LOW 
     --------------------------- --|------------------------------------------
       | d | c | b | a | fmt |retPC| ebp | locals
     ----------------|----------------|---------------------------------------
                     |                | 
                int *ip            CPU.ebp
      
         1. Let char *cp point at the format string

         2. Let int *ip  point at the first item to be printed on stack:

      *************** ALGORITHM ****************
       Use cp to scan the format string:
           spit out each char that's NOT %
           for each \n, spit out an extra \r

       Upon seeing a %: get next char, which must be one of 'c','s','u','d', 'o','x'
       Then call YOUR

            putchar(*ip) for 'c';
            prints(*ip)  for 's';
            printu(*ip)  for 'u';
            printd(*ip)  for 'd';
            printo(*ip)  for 'o';
            printx(*ip)  for 'x';

       Advance ip to point to the next item on stack.

    After implementing your myprintf() function, write C program to test your 
    myprintf() function first. Then use it in the assignment.
    =========================================================================






