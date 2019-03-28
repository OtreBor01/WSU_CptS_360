360 PROJECT
===========

                        CPTS 360 PROJECT

                     Date Assigned: 3-7-2019

      1. DUE and DEMO: Level-1  : 4-05-2019;  QUIZ: 4-9-2019
                       Level-2-3: 4-25-2019;  DEMO & Interview with KCW 
                                  
         For each DEMO: Turn in a hard copy of your work.

      2. May work in 2-person teams. However, a comprehensive INTERVIEW will 
         be given to EACH individual during the DEMO. The INTERVIEW results
         will be a major part of your final grades.

           ----------------  Project grading standards: --------------------
              Full credit   : Working AND can EXPLAIN your work.
              At least 1/2  : Partially working but can EXPLAIN your work.
              0 and REPORT case to EECS: working but CANNOT EXPLAIN code.
           -----------------------------------------------------------------

                         A. OBJECTIVE:
       Design and implement a Linux-compatible EXT2 file system.

                         B. SPECIFICATIONS:
    1. Files:
       Files are exactly the same as they are in the Linux file system, i.e.
       we shall use the same EXT2 file system data structures for

          SuperBlock, GroupDescriptor, Bitmaps, Inode, Directory

       EXCEPTIONS:
          Only DIR and REG file types; no SPECIAL files.
          File size : No triple-indirect blocks.
       
    2. Disks:
        Disks are "virtual disks" simulated by Linux files.  
        Disk I/O are simulated by Linux read()/write() operations on a BLKSIZE 
        basis. You may use Linux's mke2fs to create virtual disks

    3. File names:
       As in Unix, each file is identified by a pathname, e.g. /a/b/c or x/y/z.

       If a pathname begins with "/",  it's relative to the / directory.
       Otherwise, it's relative to the Current Working Directory (CWD) of the 
       running process (see Processes below). 

    4. Processes:
       Every file operation is performed by a process.
       The simulator starts with TWO processes:
           A process P1 with uid=0 (for SUPERUSER), and
           A process P2 with uid=1 (for ordinary user).
       The CWD of both processes are initially at the root directory /
      
       P1 runs first. P2 is in a readyQueue, which contains processes that are 
       ready to run.

       All processes executes the same code (i.e. the FS project), in which it

           loop forever{
                prompt for a command;
                execute the command;
           }
     
       Each command is performed by the current running process. A switch command
       changes the running process (to another READY process).

    5. File System Commands and Operations:
       File operations will be executed as commands. The required commands are 
       listed below. LEVEL 1 is the MINIMUM requirements for passing.
        
                  -------  LEVEL 1 ------------ 
                   mount_root;
                   mkdir, rmdir, ls, cd, pwd;
                   creat, link,  unlink, symlink
                   stat,  chmod, touch;

                  -------  LEVEl 2 -------------
                   open,  close,  read,  write
                   lseek  cat,    cp,    mv

                  -------  LEVEl 3 ------------ 
                   mount, umount
                   File permission checking
                  -----------------------------
          
       All commands work exactly the same way as they do in Unix/Linux. 

    ============================================================================
                           SAMPLE PROGRAMS:
    1. OVERALL: ~cs360/samples/PROJECT
                simulator.bin: The File System Simulator for EXT2 FS. 
                diskimage    : a virtual FD disk containing DIRs and FILEs
      Download and run the simulator on diskimage to see what I expect of YOUR work.

    ============================================================================

                         ~cs360/samples/PROJECT/:
     
                           LEVEL-1 STEPS

        mkdisk : create a virtual disk named disk with an EXT2 FS by Linux
        step0  : mount disk as the root FS; ls, pwd, cd, quit
        step1  : mount disk as the root FS; ls,pwd,cd, mkdir, creat
        step2  : ls,pwd,cd; mkdir,creat;  rmdir
        step3  : ls,pwd,cd,mkdir,creat,rmdir;  link, symlink, unlink
