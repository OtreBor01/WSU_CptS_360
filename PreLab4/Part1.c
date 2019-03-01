/**** C4.1.c file: compute matrix sum by threads ***/
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>

#define  M   4
#define  N   50000

int A[M][N];
long sum[M];

long total;

void *func(void *arg)              // threads function
{
   int row = 0, mysum = 0;
   pthread_t tid = pthread_self(); // get thread ID number
   row = (int)arg;                 // get row number from arg
   printf("Thread %d computes sum of row %d\n", row, row);
   for (int j=0; j < N; j++){     // compute sum of A[row]in global sum[row]
       mysum += A[row][j];
   }
   sum[row] = mysum;
   printf("Thread %d Done: sum[%d] = %ld\n", row, row, sum[row]);
   pthread_exit((void *)0); // thread exit: 0=normal termination
}

int main (int argc, char *argv[])
{
   struct timeval t1, t2;
   gettimeofday(&t1, NULL);
   pthread_t thread[M];      // thread IDs
   printf("main: initialize A matrix\n");
   for (int i=0; i < M; i++){
     for (int j=0; j < N; j++){
       A[i][j] = i + j + 1;
     }
   }
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
   printf("main: compute and print total: ");
   for (int i=0; i < M; i++){
       total += sum[i];
   }
   printf("%ld\n", total);
   gettimeofday(&t2, NULL);
   t2.tv_sec -= t1.tv_sec;
   t2.tv_usec -= t1.tv_usec;
   //Show total time of running PROGRAM CODE in sec=x  usec=yyyyyyy
   printf ("Time: %ld.%06ld seconds\n", t2.tv_sec, t2.tv_usec);
   pthread_exit(NULL);
}
