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
	struct TCB *parent_thread;
	struct TCB *child_thread;
};

typedef struct TCB* TCB_t;

int do_preempt;

//find specific state of thread
int find_item(queue_t q, void *data, void *arg) //ERROR STRUCT TO PTR 
{
	//it is already a ptr no need for *a
    TCB_t a = (TCB_t)data;
    int match = (int)(long)arg;
    (void)q; //unused

    if (a->state == match)
        return 1;

    return 0;
}
//find specifc tid of thread
static int tid_search(queue_t q, void *data, void *arg)
{
    TCB_t a = (TCB_t)data;
    int match = (int)(long)arg;
    (void)q; //unused

    if (a->tid == match)
        return 1;

    return 0;
}

//int tid_search(queue_t q, void*data)

//shared queue
queue_t lifecycle_q;

//current running thread
TCB_t running_thread;

//Number of threads created
int thread_count;

TCB_t threads[USHRT_MAX];


int uthread_start(int preempt)
{
	do_preempt = preempt;
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
	
	if (do_preempt == 1) preempt_start();
	return 0;
}

int uthread_stop(void)
{
	if (do_preempt == 1) preempt_stop();
	void* dummy_thread;

	running_thread = NULL;
	thread_count = 0;
	//check for length

	//check to see if main thread is running
	if(threads[0]->state != RUNNING){
		return -1;
	}
	//check queue if only has one element in queue
	if(queue_length(lifecycle_q) != 1){
			return -1;
	}
	queue_dequeue(lifecycle_q, &dummy_thread);
	return queue_destroy(lifecycle_q);
}

int uthread_create(uthread_func_t func)
{
	if (do_preempt == 1) preempt_disable();
	//allocate memory into the heap
	threads[thread_count] = (TCB_t) malloc(sizeof(TCB_t));
	
	//memory allocation failure
	if (threads[thread_count] == NULL) return -1;
	
	//for every instance that a tread is created +1
	//main = 0, t1 = 1
	threads[thread_count]->tid = thread_count;
	

	//Overflowing TID
	if(thread_count > USHRT_MAX) return -1;
	
	//start threads in ready state
	threads[thread_count]->state = READY;
	//allocated stack for thread
	threads[thread_count]->top_of_stack = uthread_ctx_alloc_stack();
	//extract thread context
	uthread_ctx_init(&(threads[thread_count]->thread_context), threads[thread_count]->top_of_stack, func);

	queue_enqueue(lifecycle_q, (void*) threads[thread_count]);

	thread_count++;

	if (do_preempt == 1) preempt_enable();

	return threads[thread_count-1]->tid;

}

void uthread_yield(void)
{
	if (do_preempt == 1) preempt_disable();
	//move running process to queue; yields its time
    TCB_t prev_thread, next_thread = NULL;

    prev_thread = running_thread;
    if (prev_thread->state == RUNNING){
        prev_thread->state = READY; 
    }
    queue_enqueue(lifecycle_q, (void *) prev_thread);
    queue_iterate(lifecycle_q, find_item, (void *) READY, (void**)&next_thread);
    if (next_thread == NULL) return; // no more threads to run
    queue_delete(lifecycle_q, next_thread);
    running_thread = next_thread;
    // context switch
    uthread_ctx_switch(&(prev_thread->thread_context), &(running_thread->thread_context));
	if (do_preempt == 1) preempt_enable();
}

uthread_t uthread_self(void)
{
	//extracts the TID of the currently running state
	return running_thread->tid;
} 

void uthread_exit(int retval)
{
	if (do_preempt == 1) preempt_disable();
    // set state to zombie and set retval, enqueue
	running_thread->state = ZOMBIE;
    running_thread->retval = retval;
	//setting parent to READY
	running_thread->parent_thread->state = READY;
	if (do_preempt == 1) preempt_enable();
    uthread_yield();
}

int uthread_join(uthread_t tid, int *retval)
{
	if (do_preempt == 1) preempt_disable();
	TCB_t curr_trd = NULL;

	//check to see if we're joining main (main runs until join)
	if(tid == 0){
		return -1;
	}
	if(uthread_self() == tid)
	{
		return -1;
	}
	//use tid_search to find specific tid and grab any curr thread that has the specific tid
	//stores tid into curr_trd->tid
	queue_iterate(lifecycle_q, tid_search, (void *)(long)tid, (void**)&curr_trd);
	//check to see if thread is a zombie or not
	//If T2 is already dead, T1 can collect T2 right away.
	//check if T2 is dead
	if(curr_trd->state == ZOMBIE){
		//just delete curr_trd from queue, free memory for it and store its return value in retval
		*retval = curr_trd->retval;
		queue_delete(lifecycle_q, (void*) curr_trd);
		free(curr_trd);
		//remember retval is the parameter passed to this func which is meant to hold the return value of the thread
		if (do_preempt == 1) preempt_enable();
		return 0;
		//then you can return
	}else{
		//If T2 is still an active thread, T1 must be blocked (i.e. it cannot be scheduled to run anymore) until T2 dies. When T2 dies, T1 is unblocked and collects T2.
		//make the parent thread the running process
		curr_trd->parent_thread =  running_thread;
		if(running_thread->state != BLOCKED){
			running_thread->state = BLOCKED;
		}
		//wait till T2 dies
		if (do_preempt == 1) preempt_enable();
		while (curr_trd->state != ZOMBIE){
			uthread_yield();
		}
		if (do_preempt == 1) preempt_disable();
		
		//When T2 dies, T1 is unblocked and collects T2.
		running_thread->state = READY;
		*retval = curr_trd->retval;
		//once T2 is collected, we get rid of it
		queue_delete(lifecycle_q, (void*) curr_trd);
		free(curr_trd);
		
	}
	if (do_preempt == 1) preempt_enable();
	return -1;
}