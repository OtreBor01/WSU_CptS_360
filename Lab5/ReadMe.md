Programming Assignment \#7: Mount Root Operation of FS
======================================================

DUE: 3-28-2019 in class; turn in a hard copy
============================================

QUIZ on 4-02-2019 in class
==========================

                        A. REQUIREMENTS: 
    =======================================================================
    Write a C program to start up the FS project:

    1. Initialize FS data structures
    2. mount the root file system
    3. ls the root directory /
    4. cd and pwd
    =======================================================================

                         B. HELP information

    1. Download files from ~samples/MOUNTROOT
       README

    2. FS Level-1 Data Structures

    PROC* running           MINODE *root                          
          |                          |                 ||*********************
          V                          |  MINODE         || 
        PROC[0]                      V minode[64]      ||         Disk dev
     =============  |-pointerToCWD-> ==== 0 =====      ||   ==================
     |nextProcPtr|  |                |  INODE   |      ||   |     INODEs   
     |pid = 0    |  |                | -------  |      ||   ================== 
     |uid = 0    |  |                | (dev,2)  |      || 
     |cwd --------->|                | refCount=3      ||*********************
     |           |                   | dirty=0  |
     |fd[8]      |                   | mounted=1|         
     | ------    |                   | mptr     |
     |           |                   |----------| 
     | - ALL 0 - |                   |==========|         
     | ------    |                   |  INODE   |          
     | ------    |                   | -------  |   
     =============                   | (dev,ino)|   
                                     | refCount |  
       PROC[1]          ^            | dirty    |   
        pid=1           |            |          |
        uid=1           |            |          |
        cwd ----> root minode        |==========|  


    3. init() // Initialize data structures of LEVEL-1:
       {
         (1). 2 PROCs, P0 with uid=0, P1 with uid=1, all PROC.cwd = 0
         (2). MINODE minode[64]; all with refCount=0
         (3). MINODE *root = 0;
       }

    4. util.c file contains functions: getino(pathname); iget(dev, ino); iput(mip);
       Their usage has the following pattern:

         int ino     = getino(char *pathname);  
         MINODE *mip = iget(dev, ino);

                       // USE the INODE in minode

                       iput(mip)

    5. mount_root()  // mount root file system, establish / and CWDs
       {
          open device for RW (use the file descriptor as dev for the opened device)
          read SUPER block to verify it's an EXT2 FS
          record nblocks, ninodes as globals

          read GD0; record  bamp, imap, inodes_start as globals;
          
          root = iget(dev, 2);    // get root inode 
       
          Let cwd of both P0 and P1 point at the root minode (refCount=3)
              P0.cwd = iget(dev, 2); 
              P1.cwd = iget(dev, 2);

          Let running -> P0.
        }


    6.int main()
      {
         init();
         mount_root();

         while(1){
            //  ask for a command line = "cmd [pathname]"
            //  cmd=ls:
                    ls(pathname);
            //  cmd=cd:
                    chdir(pathname);
            //  cmd=pwd
                    pwd(running->cwd)
                cmd=quit
                    quit();
      }       

    7. HOW TO ls [pathname]:

       You already had a ls.c program before. It
       ----------------------------------------------------------------------------
       ls_dir(dirname): use opendir() and readir() to get filenames under dirname;
                        For each filename: call 
       ls_file(filename): STAT filename; do ls on STAT
       ---------------------------------------------------------------------------
                       MODIFY YOUR ls(dirname) as
    (1).ls_dir(dirname):
          
          ino = getino(dirname);
          mip = iget(dev, ino); ===> mip points at dirname's minode
                                                             INODE  
                                                             other fields
          Get INODE.i_block[0] into a buf[ ];
          Step through each dir_entry (skip . and .. if you want)
          For each dir_entry: you have its ino and name string.
          call ls_file(ino, name)

    (2).ls_file(int ino, char *name):

          mip = iget(dev, ino); ==> mip points at minode
                                                  INODE
          INODE *ip = &(mip->INODE);
          Use ip to access INODE fields:
    ====================================================
    print d|-|l by file type:

    if (S_ISDIR(i_mode))     print    'd'
       YOU DO the file case  print    '-' 
                  LNK  case  print    'l'
    ====================================================
    print permissions:

                      876 543 210
    imode = |tttt|bbb|rwx rwx rwx|

    if (i_mode & (1 << 8)) print  'r'   else print '-'
    if (i_mode & (1 << 7)) print  'w'
    if (i_mode & (1 << 6)) print  'x'

    YOU DO the other 6 bits
    ====================================================
    print i_links_count;
    print gid, uid;
    print i_size
    print ctime(&i_mtime)
    print name string
    =====================================================

    8. HOW TO chdir(char *pathname)
       {
          if (no pathname)
             cd to root;
          else
             cd to pathname by
          (1).  ino = getino(pathname);
          (2).  mip = iget(dev, ino);
          (3).  verify mip->INODE is a DIR
          (4).  iput(running->cwd);
          (5).  running->cwd = mip;
       }

    9. HOW TO pwd()?    pwd(running->cwd):

       pwd(MINODE *wd){
          if (wd == root) print "/"
          else
             rpwd(wd);
       }
       rpwd(MINODE *wd)
       {
         if (wd==root) return;
         from i_block[0] of wd->INODE: get myino of . parentino of ..
         pip = iget(dev, parentino);
         from pip->INODE.i_block[0]: get myname string as LOCAL

         rpwd(pip);  // recursive call rpwd() with pip

         print "/%s", my_name;
       }


    10.int quit()
       {
         iput() all minodes with (refCount > 0 && DIRTY);
         exit(0); 
       }

    11. Sample Solution: ~samples/MOUNTROOT/a.out (run on mydisk)


                      Review Questions:

    1. EXPLAIN in words: What does mount root do? 

    2. Given  int ino;              // an INODE number
              INODE thisInode;      // an INODE structure
       Write C code to load the INODE of ino into thisInode

    3. Assume: MINODE *mip points at an in-memory DIR minode
                                                      INODE
                                                      dev, ino
                                                      refCount
                                                      dirty

    (1). Write C code to print its parent ino:

    (2). HOW to find the name string of this DIR INODE?
              


