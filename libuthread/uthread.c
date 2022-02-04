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

struct TCB{	
	uthread_t tid;
	int state;
	uthread_ctx_t thread_context;
	int* top_of_stack;
};

typedef struct TCB* TCB_t;

//shared queue
queue_t ready_q;

TCB_t running;
int thread_count;



int uthread_start(int preempt)
{
	//allocate memory into the heap
	TCB_t main_TCB = (TCB_t) malloc(sizeof(TCB_t));

	//keeps track of running TID
	running = main_TCB;
	//How many threads are created
	thread_count = 1;
	
	main_TCB->tid = 0;
	main_TCB->state = RUNNING;
	uthread_ctx_init(&(main_TCB->thread_context), main_TCB->top_of_stack, main());
	main_TCB->top_of_stack = (int *) uthread_ctx_alloc_stack();
	
	ready_q = queue_create();
	if (ready_q == NULL){
		return -1;
	}
	//add main TCB into the top of queue
	queue_enqueue(ready_q, (void*) main_TCB);
	//success
	return 0;
}

int uthread_stop(void)
{
	/* TODO */
	void** dummy;

	running = NULL;
	thread_count = 0;
	
	while (ready_q->size != 0){
		queue_dequeue(ready_q, &dummy);
	}
	return queue_destroy(ready_q);
}

int uthread_create(uthread_func_t func)
{
	/* TODO */
	//allocate memory into the heap
	TCB_t thread = (TCB_t) malloc(sizeof(TCB_t));
	
	//memory allocation failure
	if (thread == NULL) return -1;
	
	//for every instance that a tread is created +1
	//main = 0, t1 = 1
	thread->tid = thread_count;
	thread_count++;

	//Overflowing TID
	if(thread_count > USHRT_MAX) return -1;
	
	//start threads in ready state
	thread->state = READY;
	//extract thread context
	uthread_ctx_init(&(thread->thread_context), thread->top_of_stack, (*func));
	
	//context creation failure
	if(thread->thread_context == NULL) return -1;
	thread->top_of_stack = (int *) uthread_ctx_alloc_stack();

	queue_enqueue(ready_q, (void*) thread);
	
	return thread->tid;

}

void uthread_yield(void)
{
	
	//move running process to ready queue; yields its time
	running->state = READY; 
	queue_enqueue(ready_q, (void *) running);
	queue_dequeue(ready_q, &running);

}

uthread_t uthread_self(void)
{
	//extracts the TID of the currently running state
	return running->tid;
}

void uthread_exit(int retval)
{
	/* TODO */
}

int uthread_join(uthread_t tid, int *retval)
{
	/* TODO */

	while(1){
		//no more threads to run in system
		if(ready_q == NULL){
			break;
		}
	}
	return -1;
}

