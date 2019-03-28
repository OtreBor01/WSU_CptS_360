                 CS360 LAB#3 PreWork DUE: 2-14/15
## 1. Description:
  * This Assinment was designed to help us further understand how an operating system is a multitasking system. In an operating system, tasks are also called processes. For all practical purposes, the terms task and process can be used interchangeably. In Chap. 2, we defined an execution image as a memory area containing the execution’s code, data and stack. Formally, we define process as A process is the execution of an image. It is a sequence of executions regarded by the OS kernel as a single entity for using system resources. System resources include memory space, I/O devices and, most importantly, CPU time. In an OS kernel, each process is represented by a unique data structure, called the Process Control Block (PCB) or Task Control Block (TCB), etc. In this book, we shall simply call it the PROC structure. Like a personal record, which contains all the information of a person, a PROC structure contains all the information of a process. In a real OS, the PROC structure may contain many fields and quite large. To begin with, we shall define a very simple PROC structure to represent processes.

## 2. Execution:
  * To run code, cd into the 'PreLab3' and run './mk'
    NOTE: LAB#3 PreWork require you to turn in a harcopy of your code

## 3. Deatils:


    Modify MTX2 to the following:

    (1). char *myname = "YOUR_NAME";  // your name 

         Rewrite the body() function as 
                 int body(char *myname){ printf("%s", myname);  .....  }
         Modify kfork() to do the following:

            When a new proc starts to execute body(), print the myname string,
                 which should be YOUR name.

            When body() ends, e.g. by a null command string, retrun to do_exit();

      HINT: AS IF the proc has CALLED body(), passing myname as parameter.

    (2). Modify kfork() to implement process family tree as a binary tree
           In the body() function, print the children list of the running proc.

    (3). Implement  kexit(int exitValue):
           P1 never die;
           give away children, if any, to P1
           record exitValue in proc's exitCode;
           become a ZOMBIE;
           wakeup parent (by parent proc address);
           tswitch();

    (4). Implement  int wait(int *status):
           return -1 for ERROR if no child;

           while(1){
              if (found a ZOMBIE child){
                 copyt ZOMBIE child exitCode to *status;
                 bury the ZOMBIE child (put into freeList as FREE);
                 return ZOMBIE child pid;
              }
              sleep on OWN proc address;
            }

    (5). Add a "wait" command and a do_wait() function to allow (running) proc to 
         wait for a ZOMBIE child; print the ZOMBIE child's pid and exit status.

     
    4. DEMO your work on 2-14/15.

    5. Sample solution: samples/LAB3/LAB3pre.bin

              


