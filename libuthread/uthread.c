#include <assert.h>
#include <limits.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "uthread.h"
#include "queue.h"

/* TODO */

#define RUNNING 0
#define READY 1
#define BLOCKED 2

int running;
int thread_count;

struct TCB{	
	uthread_t tid;
	int state;
	uthread_ctx_t thread_context;
	void* top_of_stack;
};



int uthread_start(int preempt)
{
	struct TCB main_TCB;

	/* TODO */
	//keeps track of running TID
	running = 0;
	//How many threads are created
	thread_count = 1;
	
	main_TCB.tid = 0;
	main_TCB.state = RUNNING;
	uthread_ctx_init(&main_TCB.context, main_TCB.top_of_stack, main);

	
	queue_t ready = queue_create();
	if (ready == NULL){
		return -1;
	}
	//success
	return 0;
}

int uthread_stop(void)
{
	/* TODO */
	return -1;
}

int uthread_create(uthread_func_t func)
{
	/* TODO */
	
	return -1;
}

void uthread_yield(void)
{
	/* TODO */
}

uthread_t uthread_self(void)
{
	/* TODO */
	return -1;
}

void uthread_exit(int retval)
{
	/* TODO */
}

int uthread_join(uthread_t tid, int *retval)
{
	/* TODO */
	return -1;
}

