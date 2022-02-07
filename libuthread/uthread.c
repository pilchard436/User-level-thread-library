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

#define RUNNING 0
#define READY 1
#define BLOCKED 2
#define ZOMBIE 3

struct TCB{	
	uthread_t tid;
	int state;
	uthread_ctx_t thread_context;
	void* top_of_stack;
    int retval;
};

typedef struct TCB* TCB_t;

int find_item(queue_t q, void *data, void *arg)
{
    TCB_t *a = (TCB_t)data;
    int match = (int)arg;
    (void)q; //unused

    if (*a->state == match)
        return 1;

    return 0;
}

//shared queue
queue_t lifecycle_q;

//current running thread
TCB_t running_thread;

//Number of threads created
int thread_count;

TCB_t threads[USHRT_MAX];


int uthread_start(int preempt)
{
	/*Initalize Scheduling queue*/
	lifecycle_q = queue_create();
	
	if (lifecycle_q == NULL){
		return -1;
	}

	/*Create Main Thread*/
	//allocate memory into the heap
	threads[0] = (TCB_t) malloc(sizeof(TCB_t));

	//keeps track of running TID
	running_thread = threads[0];
	//How many threads are created
	thread_count = 1;
	
	threads[0]->tid = 0;
	threads[0]->state = RUNNING;
	//context is already dont automatically for us
	//the context running is already in main, dont need to 

	//allocated memory for thread context 
	threads[0]->top_of_stack = uthread_ctx_alloc_stack();
	//set curthread to init running thread
	//add main TCB into the top of queue
	queue_enqueue(lifecycle_q, (void*) threads[0]);
	//success
	return 0;
}

int uthread_stop(void)
{
	void* dummy_thread;

	running_thread = NULL;
	thread_count = 0;
	//check for length

	//check to see if main thread is running
	if(threads[0]->state != RUNNING){
		return -1;
	}
	//check queue if only has one element in queue
	if(queue_length != 1){
			return -1;
	}
	queue_dequeue(lifecycle_q, &dummy_thread);
	return queue_destroy(lifecycle_q);
}

int uthread_create(uthread_func_t func)
{
	/* TODO */
	//allocate memory into the heap
	threads[thread_count] = (TCB_t) malloc(sizeof(TCB_t));
	
	//memory allocation failure
	if (threads[thread_count] == NULL) return -1;
	
	//for every instance that a tread is created +1
	//main = 0, t1 = 1
	threads[thread_count]->tid = thread_count;
	thread_count++;

	//Overflowing TID
	if(thread_count > USHRT_MAX) return -1;
	
	//start threads in ready state
	threads[thread_count]->state = READY;
	//allocated stack for thread
	threads[thread_count]->top_of_stack = uthread_ctx_alloc_stack();
	//extract thread context
	uthread_ctx_init(&(threads[thread_count]->thread_context), threads[thread_count]->top_of_stack, func);

	queue_enqueue(lifecycle_q, (void*) threads[thread_count]);
	
	return threads[thread_count]->tid;

}

void uthread_yield(void)
{
	//move running process to queue; yields its time
    TCB_t prev_thread, next_thread = NULL;

    prev_thread = running_thread;
    if (prev_thread->state = RUNNING){
        prev_thread->state = READY; 
    }
    queue_enqueue(lifecycle_q, (void *) prev_thread);
    queue_iterate(lifecycle_q, find_item, (void *) READY, (void**)&next_thread);
    if (next_thread == NULL) return; // no more threads to run
    queue_delete(lifecycle_q, next_thread);
    running_thread = next_thread;
    // context switch
    uthread_ctx_switch(prev_thread->thread_context, running_thread->thread_context);
	
}

uthread_t uthread_self(void)
{
	//extracts the TID of the currently running state
	return running_thread->tid;
} 

void uthread_exit(int retval)
{
    // set state to zombie and set retval, enqueue
	running_thread->state = ZOMBIE;
    running_thread->retval = retval;
    uthread_yield();
}

int uthread_join(uthread_t tid, int *retval)
{
	/* TODO */

	while(1){
		//no more threads to run in system
		if(queue_length(lifecycle_q) == 0){
			break;
		}
        uthread_yield();
	}
	return -1;
}

