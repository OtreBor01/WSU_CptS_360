/*********** t.c file of A Multitasking System *********/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "type.h"

PROC proc[NPROC];      // NPROC PROCs
PROC *freeList;        // freeList of PROCs 
PROC *readyQueue;      // priority queue of READY procs
PROC *running;         // current running proc pointer
PROC *sleepList;       // list of SLEEP procs
#include "queue.c"     // include queue.c file
#include "wait.c"      // include wait.c file

/*******************************************************
  kfork() creates a child process; returns child pid.
  When scheduled to run, child PROC resumes to body();
********************************************************/
extern int body(char *); 
int init(); int scheduler();
void DisplayBinaryTree();
int do_sleep(); //int sleep(int) in wait.c
int do_wakeup(); //int wakeup(int) in wait.c
int do_switch();
int do_exit(); void kexit(int);//int kexit(int) in wait.c
int do_wait(); int kwait(int *); PROC *findZombieChild(PROC*);
int do_kfork(); int kfork();

char *status[ ] = {"FREE", "READY", "SLEEP", "ZOMBIE"};

int do_ps()
{
  int i;
  PROC *p;
  printf("PID  PPID  status\n");
  printf("---  ----  ------\n");
  for (i=0; i<NPROC; i++){
    p = &proc[i];
    printf(" %d    %d    ", p->pid, p->ppid);
    if (p == running)
      printf("RUNNING\n");
    else
      printf("%s\n", status[p->status]);
  }
}

int do_jesus()
{
  int i;
  PROC *p;
  printf("Jesus perfroms miracles here\n");
  for (i=1; i<NPROC; i++){
    p = &proc[i];
    if (p->status == ZOMBIE){
      p->status = READY;
      enqueue(&readyQueue, p);
      printf("raised a ZOMBIE %d to live again\n", p->pid);
    }
  }
  printList("readyQueue", readyQueue);
}
void DisplayBinaryTree(){
	for(int i = 0; i < NPROC; i++){
		if(&proc[i] == running || proc[i].status == READY){
			printf("| parent = P%d | children = ", proc[i].pid);
			PROC *child = proc[i].child;
			char stars[100] = "*****************************";
			while(child){
				printf("P%d -> ", child->pid);
				strcat(stars, "******");
				child = child->sibling;
			}
			printf("NULL |\n");
			strcat(stars, "****");
			puts(stars);
		}
	}
} 
/*
int body(char *myname)// process body function
{
  int c;
  char cmd[64];
  printf("proc %d starts from body(): myname = %s\n", running->pid, myname);
  while(1){
    printf("***************************************\n");
    printf("proc %d running: parent=%d  ", running->pid, running->ppid);
    printChild("childList", running->child);
    printList("readyQueue", readyQueue);
    printSleep("sleepList ", sleepList);
    //puts("Process Family Tree as a Binary Tree");
    //DisplayBinaryTree();
    menu();
    printf("enter a command : ");
    fgets(cmd, 64, stdin);
    cmd[strlen(cmd)-1] = 0;
    if (strcmp(cmd, "ps")==0)
      do_ps();
    if (strcmp(cmd, "fork")==0)
      do_kfork();
    if (strcmp(cmd, "switch")==0)
      do_switch();
    if (strcmp(cmd, "exit")==0)
      do_exit();
    if (strcmp(cmd, "jesus")==0)
      do_jesus();
   	if (strcmp(cmd, "sleep")==0)
      do_sleep();
   	if (strcmp(cmd, "wakeup")==0)
      do_wakeup();
   	if (strcmp(cmd, "wait")==0)
      do_wait();
   	if (cmd == NULL)
   	  return do_exit();
 }
}
*/

int do_kfork()
{
   int child = kfork();
   if (child < 0)
      printf("kfork failed\n");
   else{
      printf("proc %d kforked a child = %d\n", running->pid, child); 
      printList("readyQueue", readyQueue);
   }
   return child;
}
int kfork()
{
  int  i;
  PROC *p = dequeue(&freeList);
  if (!p){
     printf("no more proc\n");
     return(-1);
  }
  /* initialize the new proc and its stack */
  p->status = READY;
  p->priority = 1;       // ALL PROCs priority=1,except P0
  p->ppid = running->pid;
  //Additions By Austin ???
  p->parent = running;
  PROC * child = running->child, *prev = NULL;
  if(child){
  	prev = child;
  	child = child->sibling;
  }
  if(prev){ prev->sibling = p; }
  else{ running->child = p; }
  //**********
  /************ new task initial stack contents ************
   kstack contains: |retPC|eax|ebx|ecx|edx|ebp|esi|edi|eflag|
                      -1   -2  -3  -4  -5  -6  -7  -8   -9
  **********************************************************/
  for (i=1; i<12; i++)               // zero out kstack cells
      p->kstack[SSIZE - i] = 0;
  p->kstack[SSIZE-1] = (int)("Austin Marino");
  p->kstack[SSIZE-3] = (int)body;    // retPC -> body()
  p->ksp = &(p->kstack[SSIZE - 11]);  // PROC.ksp -> saved eflag 
  enqueue(&readyQueue, p);           // enter p into readyQueue
  return p->pid;
}

int do_switch()
{
   tswitch();
}

int do_exit()
{
  kexit(running->pid);  // exit with own PID value 
}
void kexit(int exitValue){
	PROC *P1 = &proc[1], *child = NULL, *prev = NULL;
	if(P1 != running){
		printf("proc %d in kexit(), value=%d\n", running->pid, exitValue);
		//Traverse to end of P1's child list
		child = P1->child;
		while(child){
			prev = child;
			child = child->sibling;
		}
		// give away children, if any, to P1
		if(prev){prev->sibling = running->child; } //P1 has 1 or more children
		else{ P1->child = running->child; } //P1 has no chidren
		//Assign all running proc's children to P1 as parent proc
		child = running->child;
		while(child){
			child->parent = P1;	
			child->ppid = P1->pid;
			child = child->sibling;
		}
		running->exitCode = exitValue; //record exitValue in proc's exitCode;
		running->status = ZOMBIE; //become a ZOMBIE;
		int ppa = (running->parent != NULL) ? (int)running->parent : (int)P1;//parent proc address
		wakeup(ppa); //wakeup parent (by parent proc address);
		tswitch();
    }
    else{ puts("P1 Never Dies"); }
}

int do_sleep()
{
  int event;
  printf("enter an event value to sleep on : ");
  scanf("%d", &event); getchar();
  sleep(event);
}

int do_wakeup()
{
  int event;
  printf("enter an event value to wakeup with : ");
  scanf("%d", &event); getchar(); 
  wakeup(event);
}

int do_wait()
{
  int exitCode = 0;
  int child = kwait(&exitCode);
  if(child != -1){
  	printf("proc%d buried a ZOMBIE child = %d status = %d\n", running->pid, child, exitCode);
  }
  else{ puts("wait error: no child"); }
  
}
PROC *findZombieChild(PROC *p){
	if(p){
		if (p->status == ZOMBIE){ return p; }
		return findZombieChild(p->sibling);
	} return NULL;
}
/******* Algorithm of kwait() *******/
int kwait(int *statusCode)
{
	PROC *childProc = running->child;
  	printf("proc %d waits for a ZOMBIE child\n", running->pid);
	while(childProc){
		// caller has ZOMBIE child
		if((childProc = findZombieChild(childProc))){ //found a ZOMBIE child
			int pid = childProc->pid; //get ZOMBIE child pid
			*statusCode = childProc->exitCode; //copy ZOMBIE child exitCode to *statusCode;
			childProc->status = FREE; //put into freeList as FREE
			//removes zombie proc from running proc's child list
			PROC *cur = running->child, *prev = NULL;
			while(cur){
				if(cur == childProc){break;}
				prev = cur; cur = cur->sibling;
			}
			if(prev){ prev->sibling = childProc->sibling;}
			else{ running->child = NULL; }
			enqueue(&freeList, childProc);//bury the ZOMBIE child (put its PROC back to freeList)
			return pid; //return ZOMBIE child pid;
		}
		//**** has children but none dead yet ****
		sleep((int)running); // sleep on its PROC address
	}
	return -1; // caller has no children
}
// initialize the MT system; create P0 as initial running process
int init() 
{
  int i;
  PROC *p;
  for (i=0; i<NPROC; i++){ // initialize PROCs
    p = &proc[i];
    p->pid = i;            // PID = 0 to NPROC-1  
    p->status = FREE;
    p->priority = 0;      
    p->next = p+1;
  }
  proc[NPROC-1].next = 0;  
  freeList = &proc[0];     // all PROCs in freeList     
  readyQueue = 0;          // readyQueue = empty
  sleepList = 0;           // sleepList = empty
  // create P0 as the initial running process
  p = running = dequeue(&freeList); // use proc[0] 
  p->status = READY;
  p->priority = 0;
  p->ppid = 0;// P0 is its own parent

  printList("freeList", freeList);
  printf("init complete: P0 running\n"); 
}


/*********** scheduler *************/
int scheduler()
{ 
  printf("proc %d in scheduler()\n", running->pid);
  if (running->status == READY)
     enqueue(&readyQueue, running);
  printList("readyQueue", readyQueue);
  running = dequeue(&readyQueue);
  printf("next running = %d\n", running->pid);  
}
