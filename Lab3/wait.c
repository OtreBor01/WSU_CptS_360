int tswitch();

int ksleep(int event)
{
  printf("proc %d going to sleep on event=%d\n", running->pid, event);

  running->event = event;
  running->status = SLEEP;
  enqueue(&sleepList, running);
  printList("sleepList", sleepList);
  tswitch();
}

int wakeup(int event)
{
	PROC *temp, *p;
 	temp = 0;
	printList("sleepList", sleepList);
	while (p = dequeue(&sleepList)){
		if (p->event == event){
	 		printf("wakeup %d\n", p->pid);
	 		p->status = READY;
	 		enqueue(&readyQueue, p);
     	}
     	else{
			enqueue(&temp, p);
     	}
  	}
  	sleepList = temp;
  	printList("sleepList", sleepList);
}
/*
int kexit(int exitValue)
{
  printf("proc %d in kexit(), value=%d\n", running->pid, exitValue);
  running->exitCode = exitValue;
  running->status = ZOMBIE;
  tswitch();
}
*/

/*
int kwakeup(int event){
	PROC *p = sleepList, *prev = NULL;
	while(p){ //for each PROC *p in sleepList
		if (p->event == event){//if p is sleeping for the event
			//delete p from sleepList;
			if(prev){ prev->next = p->next; }
			else{ sleepList->next = p->next; }
			p->status = status[1]; //p->status = READY, make p READY to run again
			enqueue(&readyQueue, p); // enter p into readyQueue
		}
		prev = p;
		p = p->next;
	}
}
*/

  
