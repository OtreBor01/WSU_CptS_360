/* itimer.c program */
//The program will print   timer_handler: signal=14   once every second.
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>
#include <stdbool.h>

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
#define START 500000
#define INTERVAL 1000
#define USEC_IN_SEC 1000000
#define STOPTICK 30
int hh, mm, ss, tick;
long uss = 0;
struct itimerval t;
bool timerStarted = false;

void timer_handler (int sig)
{
	int startSec = ss;
	if(timerStarted == false){ 
		uss = START; 
		timerStarted = true;
	}
	else{
		uss += INTERVAL;
		ss += uss / USEC_IN_SEC;
		uss = uss % USEC_IN_SEC;
		mm += ss / 60;
		ss = ss % 60;
		hh += mm / 60;
		mm = mm % 60;
	}
	if(startSec != ss){ //Display a wall clock in the form of hh : mm : ss which is updated once every second.	
		printf("Wall Clock: %02d : %02d : %02d\n", hh, mm, ss);
		tick++;
	}
	if (tick >= STOPTICK){
		exit(0);
	}
}

int main ()
{
	struct itimerval itimer;
	tick = hh = mm = ss = 0;

	signal(SIGALRM, timer_handler);

	/* Configure the timer to expire after .5 sec */
	itimer.it_value.tv_sec  = 0;
	itimer.it_value.tv_usec = START;

	/* and every 1000 usec after that */
	itimer.it_interval.tv_sec  = 0;
	itimer.it_interval.tv_usec = INTERVAL;
	// Start a VIRTUAL itimer
	setitimer(ITIMER_REAL, &itimer, NULL);
	printf("looping: enter Control-C to terminate\n");
	while (1);
}

