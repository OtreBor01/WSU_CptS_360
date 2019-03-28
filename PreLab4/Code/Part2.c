/**** PreLab4.c file: compute matrix sum by threads ***/
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h> // timeval structure
#include <pthread.h> //threads
#include <unistd.h> //sleep

//***** Defined Constants *****
#define  M   4
#define  N   50

//***** Global Variables *****
int A[M][N];
long total;
pthread_mutex_t m; //create a MUTEX m

void *func(void *arg)        // threads function
{
   int temp, mysum = 0;
   // get row number from arg
   int row = (int)arg;
   //compute row sum in mysum;
   for (int i=0; i<N; i++){
      // compute partial sum of A[row]in
	  mysum += A[row][i];
   }
   // Threads may use mutex as LOCKs to protect CRITICAL regions 
   pthread_mutex_lock(&m);
   /************** A CRITICAL REGION *******************/
   temp = total;   // get total
   temp += mysum;  // add mysum to temp  
   sleep(1);       //switch threads
   total = temp;   //  write temp to total
   /************ end of CRITICAL REGION ***************/
   pthread_mutex_unlock(&m);
   pthread_exit((void*)0);  // thread exit: 0=normal termination
}

int main (int argc, char *argv[])
{
   puts("\n\n\n***** Part 2 *****\n");
   struct timeval start, end;
   gettimeofday(&start, NULL);
   pthread_mutex_init(&m, NULL); 
   long answer = 0;
   int incorrectCount = 0;
   printf("main: initialize A matrix\n");
   for (int i=0; i < M; i++){
	 for (int j=0; j < N; j++){
	   A[i][j] = i + j + 1;
	 }
   }
   for(int count = 1; count <= 10; count++){
   	   printf("\n***** Thread Mutex Test %d *****\n", count);
   	   struct timeval t1, t2;
   	   gettimeofday(&t1, NULL);
   	   total = 0;
       pthread_t thread[M];      // thread IDs
       printf("main: create %d threads\n", M);
	   for(int i=0; i < M; i++) {
		  pthread_create(&thread[i], NULL, func, (void *)i); 
	   }
	   printf("main: try to join with threads\n");
	   for(int i=0; i < M; i++) {
	   	 int status;
		 pthread_join(thread[i], (void *)&status);
		 printf("main: joined with thread %d : status=%d\n", i, status);
	   }   
	   printf("total %d: %ld\n", count, total);
	   if(count == 1){ answer = total; }
	   else if (total != answer) { incorrectCount++; }
	   gettimeofday(&t2, NULL);
   	   t2.tv_sec -= t1.tv_sec;
   	   t2.tv_usec -= t1.tv_usec;
   	   //Show total time of running PROGRAM CODE in sec=x  usec=yyyyyyy
   	   printf("Time %d: %ld.%06ld seconds\n", count, t2.tv_sec, t2.tv_usec);
   }
   gettimeofday(&end, NULL);
   end.tv_sec -= start.tv_sec;
   end.tv_usec -= start.tv_usec;
   printf("\n***** Mutex Test Results *****\n");
   printf("Total RunTime: %ld.%06ld seconds\n", end.tv_sec, end.tv_usec);
   printf("Amount of threads with the incorrect total: %d\n", incorrectCount); 
   pthread_exit(NULL);
}
