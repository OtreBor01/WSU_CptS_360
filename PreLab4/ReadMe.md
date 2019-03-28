                      LAB Assignment # 4 Pre-Work
              
                        Due and DEMO: 2/22-2019

    1. The C4.1 program computes the sum of a MxN matrix of integers by threads.

       Use      cc c4.1.c -pthread   to generate a.out; then run a.out    

    /**** C4.1.c file: compute matrix sum by threads ***/
    #include <stdio.h>
    #include <stdlib.h>
    #include <sys/time.h>
    #include <pthread.h>

    #define  M   4
    #define  N   5

    int A[M][N], sum[M];

    int total;

    void *func(void *arg)              // threads function
    {
      int j, row, mysum;
       pthread_t tid = pthread_self(); // get thread ID number

       row = (int)arg;                 // get row number from arg
       printf("thread %d computes sum of row %d : ", row, row);
       mysum = 0;

       for (j=0; j < N; j++)     // compute sum of A[row]in global sum[row]
           mysum += A[row][j];

       sum[row] = mysum;

       printf("thread %d done: sum[%d] = %ld\n", row, row, sum[row]);
       pthread_exit((void *)row); // thread exit: 0=normal termination
    }

    // print the matrix (if N is small, do NOT print for large N)
    int print()
    {
       int i, j;
       for (i=0; i < M; i++){
         for (j=0; j < N; j++){
           printf("%4d ", A[i][j]);
         }
         printf("\n");
       }
    }

    int main (int argc, char *argv[])
    {
       pthread_t thread[M];      // thread IDs
       int i, j, status;

       printf("main: initialize A matrix\n");

       for (i=0; i < M; i++){
         for (j=0; j < N; j++){
           A[i][j] = i + j + 1;
         }
       }

       print();

       printf("main: create %d threads\n", M);
       for(i=0; i < M; i++) {
          pthread_create(&thread[i], NULL, func, (void *)i); 
       }

       printf("main: try to join with threads\n");
       for(i=0; i < M; i++) {
         pthread_join(thread[i], (void *)&status);
         printf("main: joined with thread %d : status=%d\n", i, status);
       }

       printf("main: compute and print total : ");
       for (i=0; i < M; i++)
           total += sum[i];
       printf("tatal = %ld\n", total);

       pthread_exit(NULL);
    }
    ============================================================================
    cc m1.c -pthread
    a.out
    ================== Sample Outputs ==========================================
    main: initialize A matrix
       1    2    3    4    5
       2    3    4    5    6
       3    4    5    6    7
       4    5    6    7    8
    main: create 4 threads

    thread 0 computes sum of row 0 : thread 0 done: sum[0] = 15
    thread 1 computes sum of row 1 : thread 1 done: sum[1] = 20
    thread 2 computes sum of row 2 : thread 2 done: sum[2] = 25
    thread 3 computes sum of row 3 : thread 3 done: sum[3] = 30

    main: try to join with threads
    main: joined with thread 0 : status=0
    main: joined with thread 1 : status=1
    main: joined with thread 2 : status=2
    main: joined with thread 3 : status=3

    main: compute and print total : tatal = 90
    ===========================================================================

    2. Time Service in Linux:

       #include <sys/time.h>     // timeval structure
    (1).
       /*********************************
       struct timeval {
           time_t      tv_sec;   // current seconds since 00:00:00 of 1970 
           suseconds_t tv_usec;  // microseconds in current second 
       };
       **********************************/

       struct timeval t1, t2;
       gettimeofday(&t1, NULL);
          PROGRAM CODE
       gettimeofday(&t2, NULL);

       Show total time of running PROGRAM CODE in sec=x  usec=yyyyyyy

    (2). time a.out  ==> show running time  
         ------------------------
         real    0m0.119s
         user    0m0.008s
         sys     0m0.024s 
         -----------------------

    ========================  Assignment #1: =========================
    In C4.1, change N to 500000. (but do not print the matrix)
    Use gettimeofday() to show its total running time.

    Write a program, s.c, which compute the sum of the sama matrix as in C4.1 
    by a SEQUENTIAL algorithm.

    Use both   time a.out     AND    gettimeofday() 
    to compare the run time of the 2 progams.
    ==================================================================

    3. Threads Synchronization

    (1). A CRITICAL REGION = a sequence of actions which can only be performed by
                             ONE execution entity (process or thread) at a time.

    (2). RACE CONDITION    = outcome may vary depedning the the execution orders of
                             processes or threads.

    (3). Example:  int x = 0;                                   T1          T2  
                                                            ----------   ----------
        2 threads: x = x + 1:     read x into CPU register; | read 0  
                                  add 1 to CPU register;    | add 1      | read 0
                                  write CPU register to x;  | write 1    | add 1
                                                            | (x=1)      | write 1
                                                                          (x =1)
     ==============================================================================

    Modify the threads func() as

    void *func(void *arg)        // threads function
    {
       int temp, mysum = 0;
       compute row sum in mysum;

       /************** A CRITICAL REGION *******************/
       temp = total;   // get total
       temp += mysum;  // add mysum to temp  
       
       sleep(1);       // OR for (int i=0; i<100000000; i++); ==> switch threads

       total = temp;   //  write temp to total
       /************ end of CRITICAL REGION ***************/

       pthread_exit((void*)0);  // thread exit: 0=normal termination
    }

    Let main thread print total, which is most likely INCORRECT due to 
    RACE CONDITIONs.

    4. In main(): create a MUTEX m:

          pthread_mutex_t m;
          pthread_mutex_init(&m, NULL);
      
       Threads may use mutex as LOCKs to protect CRITICAL regions 

          pthread_mutex_lock(&m);
            CRITICAL REGION
          pthread_mutex_unlock(&m);

    ============= Assignment #2 =======================================
    Use a mutex lock to protect the update of total in a CRITICAL REGION (CR)
    Verify total is CORRECT no matter how long a thread executes inside the CR.
    ===================================================================


    5. Virtual Timer and Signals 

    ====================== KERNEL MODE ====================================
                Process PROC structure
            ------------------------------
    signals: 1 2 3 4 5 6 7 ... 14 ... 31     #14 is for SIGALARM from timer
            |                 |  |   |  |    0: NO signal; 1: has signal
           ------------------------------
    handler:                  |. |           DEFault Handler: call kexit() to die
                               |
                             kexit()

    ====================== USER MODE =====================================

    (1).  void timer_hander(){ ............... }

    (2). signal(14, timer_hendler): replace DEFault handler of signal #14 with
                                    address of timer_handler() in USER mode

    (3). Every time a processs enters KERNEL mode, it checks for occurred signal. 
         If it has a #14 signal, it will return to USER mode to execute 
                                    timer_handler(), which returns to the
                                 place from where it lastly entered K mode.
    ===========================================================================
                             
    /* itimer.c program */

    #include <stdio.h>
    #include <string.h>
    #include <signal.h>
    #include <sys/time.h>

    /*************************
     struct timeval {
        time_t      tv_sec;         // seconds 
        suseconds_t tv_usec;        // microseconds 
     };
     struct itimerval {
        struct timeval it_interval; // Interval of periodic timer 
        struct timeval it_value;    // Time until next expiration
     };
    *********************/

    int hh, mm, ss, tick;

    void timer_handler (int sig)
    {
       printf("timer_handler: signal=%d\n", sig);
    }

    int main ()
    {
     struct itimerval itimer;
     tick = hh = mm = ss = 0;
     
     signal(SIGALRM, &timer_handler);
     
     /* Configure the timer to expire after 1 sec */
     timer.it_value.tv_sec  = 1;
     timer.it_value.tv_usec = 0;

     /* and every 1 sec after that */
     timer.it_interval.tv_sec  = 1;
     timer.it_interval.tv_usec = 0;

     setitimer (ITIMER_REAL, &timer, NULL);

     while (1);
    }

    The program will print   timer_handler: signal=14   once every second.


    ============= Assignment #3 =======================================
    Set the timer to fire after 0.5 second, but every 1000 usec after that.
    Display a wall clock in the form of
              hh : mm : ss
    which is updated once every second.
    ===================================================================

