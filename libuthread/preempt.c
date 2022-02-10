#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#include "private.h"
#include "uthread.h"

/*
 * Frequency of preemption
 * 100Hz is 100 times per second
 */
#define HZ 100
#define MICROSEC HZ * 100

struct sigaction sa;
struct sigaction sa_old;
struct itimerval vt; // sets timer
struct itimerval vt_old;

void alarm_handler(int alarm)
{
	(void)alarm;
	// signal handler will force the currently running thread to yield
	uthread_yield();
};

void preempt_start(void)
{
	/*Set up handler for alarm*/
	sa.sa_handler = alarm_handler;
	sigemptyset(&sa.sa_mask);
	//sa.sa_flags = 0;
	// map signal handler to signal
	sigaction(SIGVTALRM, &sa, &sa_old);

	/*Configure alarm*/
	vt.it_value.tv_sec = 0;
	vt.it_value.tv_usec = MICROSEC;
	// loop through the alarm every 10 msec
	vt.it_interval.tv_sec = 0;
	vt.it_interval.tv_usec = MICROSEC;

	if (setitimer(ITIMER_VIRTUAL, &vt, &vt_old) == -1)
	{
		perror("setitimer error");
		exit(1);
	}
}

void preempt_stop(void)
{
	// restore old sig action
	sigaction(SIGVTALRM, &sa_old, NULL);
	// restore old alarm
	setitimer(ITIMER_VIRTUAL, &vt_old, NULL);
}

void preempt_enable(void)
{
	// Unblock the signals in set—remove them from the existing mask
	sigprocmask(SIG_UNBLOCK, &sa.sa_mask, NULL);
}

void preempt_disable(void)
{
	// Block the signals in set—add them to the existing mask. In other words, the new mask is the union of the existing mask and set.
	sigprocmask(SIG_BLOCK, &sa.sa_mask, NULL);
}