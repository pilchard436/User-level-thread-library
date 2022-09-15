# User-level thread library

## Summary
This program User-level thread was aimed to help us understand threads, by 
creating a user-level thread library for Linux. The library allows the 
creation of new threads, execution of threads by using the round robin 
scheduling, the ability for threads to synchronize and join other threads, and 
provide a preemptive scheduler.

## Implementation
### 1. Queue:
In order to properly implement our user thread library functions, we needed to 
have an internal FIFO structure, namely queue. Overall, we chose to implement 
the queue using an array instead of linked list, simply because we did not 
want to create a separate node structure, which may complicate things. We 
decided that our queue would also store the index of the first item and the 
last item to simplify the implementation of other functions. A queue would 
also store its current size and the maximum capacity for similar reasons.

First we implement a helper function to check if the queue is empty, since we 
need this functionality in multiple places in our queue.

In `queue_create`, we use `malloc` to allocate the queue on the heap, because 
our queue’s size would need to dynamically change depending on how many 
items we have in the queue.

In `queue_destroy`, we simply free the queue, which will free any items in the 
queue and no memory leak would occur.

In `queue_enqueue`, we first check if the queue is full, in which case we use 
`realloc` to reallocate our `queue->arr` to some other contiguous memory 
location. Then we increment the tail, write the data to that location, and 
increment the size of the queue. In `queue_dequeue`, we write the item at 
`queue->head` into `data`, free the item in the queue itself, then increment 
the head and decrement the size to maintain consistency in the queue.

In `queue_delete`, we first find the item we wish to delete by looping through 
each item in the queue sequentially until either we find the item or we’re 
at the end of the queue, meaning the item is not found. In this case, we 
return -1 to the user to indicate this failure. If the item is found, we then 
shift each subsequent item into the previous item’s memory space, 
effectively overwriting the previous item and shifting our queue forward by 1 
item. In the end, we decrement the size as well as `queue->tail`.

In `queue_iterate`, we loop through each item in the queue by using a while 
loop. If we’re not at the end of the queue, then we pass the item to the 
function that the caller wants to call and call the function using function 
pointer. If the function returns a 1, we know that the function already did 
what it is supposed to do and we break out of the while loop and return.

Finally, for `queue_length`, we simply return `queue->size` to the caller.

When implementing our internal queue, we also included many checks such as 
checking if the queue is empty, if the proper data is passed to the function, 
if the data exists in the function, etc. Whenever an error is detected, -1 is 
returned to the caller function to ensure no segfault would occur. This makes 
our internal queue resistant to errors. 

### 2. Uthread:
In creating a uthread library at user-level we saw two approaches which would 
tackle the same problem, but would come with their own set of challenges. In 
creating a system that would keep track of threads, we needed a means of 
tracking the flow of the threads and their attributes such as states, TID, 
top_of_stack pointer, and its thread context, namely a TCB block. For the 
initial approach, we saw that having a multiple queue system would be 
beneficial as it would keep states in their separate queue until instructed to 
do otherwise. The downside of this would be how costly in memory it would be. 
Rather, we found a more efficient alternative of using a single queue, that 
would handle all non running threads regardless of state in this singular 
queue. This allows us to use a single struct for every newly created thread, 
allowing us to use multiple threads when we need to create, start, terminate 
or manipulate.

To run the process of the uthread library, we start at `uthread_start()`. Here 
we initialize the single execution flow as the main thread, which will serve 
as the mother thread that the library can later schedule. For all threads 
including main, we use `malloc` to allocate the thread on the heap, then 
enqueue it onto the queue which we call `lifecycle_q`, after all its execution 
context has been assigned. This is intended since our queue’s size would 
need to dynamically change depending on how many threads we have in the 
queue.Since main is the initial mother thread, it will be set to `RUNNING` 
until future threads are created.

After the library has been initialized, the main thread can call 
`uthread_create()` to create a new user thread. Similar to main, each newly 
created thread will use `malloc` to allocate itself on to the heap while at 
the same time incrementing the thread_count which will then in turn become its 
own TID. Prior to threads being enqueued onto the queue, they must be set to 
READY state and by using `uthread_ctx_init` we initialize the stack and the 
execution context of the new thread, which will then run in the specified 
function `func`.

Since we wish to use multithreading in our thread library, yielding threads is 
essential in allowing a fair share of resources. Threads must call 
`uthread_yield()` to ask the scheduler to pick and run the next available 
thread. In our implementation, we keep track of the thread that was initially 
running that calls yield (prev_thread) and the thread elected to run by the 
scheduler (next_thread). Followed by the queue, since we have a single queue, 
we think that it was a good time to make sure whatever initially running 
thread would be set to `READY` state and moved to the back of the queue. Since 
this thread would no longer be `RUNNING` at the moment, we will find the next 
available `READY` thread using queue iterate and a helper function 
`find_item`. This helper function finds the next thread that is ready in 
lifecycle_q and returns it to `uthread_yield()`. Finally we call 
`uthread_ctx_switch()`, which allows the uthread library to switch between two 
execution contexts. In our case, we want to switch from `prev_thread` to 
`next_thread`.

Typically, in most test files we accompany uthread_yield() with 
`uthread_join()`. Before a thread exits, it must first be joined by another 
thread, which can then collect the return value of the dead thread. For our 
implementation, we followed two scenarios in which curr_trd (T2) is still an 
active thread or it is already dead. One thing to remember is that when a 
thread is joining another thread it can never join itself. In order to get rid 
of this edge case, we used `uhtread_self()` and checked if the TID that we are 
wishing to join is not the same as the running threads. If this is not the 
case then `uthread_join()` can continue to work as expected. The next step is 
finding the thread in which we plan to join. We use `queue_iterate()` to 
occupy the curr_trd with the TID that is being joined. In order to do this, we 
created a helper function called `tid_search()` which finds the matching tid 
in lifecycle_q and occupies the curr_trd with the context belonging to that 
TID. Now we have to run through the two scenarios. By using the states, we 
check for both these cases by checking if the curr_trd is a `ZOMBIE` or not. 
Meaning that the process has completed, can return its exit status, and can 
free its memory for it from the queue. If the curr_trd is not a `ZOMBIE` 
meaning that it is an active thread either `READY`, `RUNNING`, or `BLOCKED` 
the parent thread (curr_trd->parent_thread) is now set as the running process 
and the running process T1 will be blocked. We yield curr_trd if it is ever 
`READY`. Once that T2 process has died it will be collected by T1 after it is 
unblocked, and concludes by deleting and freeing the memory of T2 thread from 
the queue.

`uthread_exit()` is called implicitly whenever a thread returns. Since the 
thread has been completed the running_thread is set `ZOMBIE` and given the 
return value of its exit status. The parent that calls join on the running 
thread is then unblocked by setting it to `READY`. Lastly, putting the 
ublocked thread to the back of the queue.

When all threads have finished executing, `uthread_stop()` will be the very 
last line in main. We check that main is the only remaining thread in the 
queue, dequeue the last thread, and destroy the queue.

### 3. Preempt:
The above multithreading works great if each thread calls `uthread_yield()` 
periodically to allow other threads to run. However if a thread wants to have 
the entire CPU to itself without calling `uthread_yield()`, there is nothing 
preventing it from doing that, which is dangerous. Preempt prevents this 
situation from happening, and it uses `SIGVTALRM` to periodically disrupt 
threads and make them yield.

In our `alarm_handler()` function, which is called whenever a signal is 
raised, we simply call `uthread_yield()`. In `preempt_start()`, we configure 
the signal handler to use our `alarm_handler()`, exclude all signals, and set 
the flag to 0. We also set it up such that whenever `SIGVTALRM` is received, 
we would use the signal handler that we defined above. Then, to determine when 
to raise a signal, we create our own timer and set it to go off every 10 ms. 
We also store the old signal handler and timer, because in `preempt_stop()`, 
we simply restore them to their original values. 

In order to not mess up or get preempted when we’re running our own uthread 
functions, we need to disable and re-enable preempt at an appropriate time. To 
enable or disable a preempt, we must call `sigprocmask()` which we use to 
fetch and/or change the signal of the calling thread. For `preempt_enable()` 
we unblock the signals and in `preempt_disable we block the signals. 

## Testing
1. queue_tester.c: In this file, for every function, we test whether it works 
correctly as well as if it handles incorrect input outputs correctly. Some 
functions had more edge cases to handle then others, so we ensured that our 
unit test would catch every bug as it should.

2. test_join.c: In this test program, we test the functionality of 
uthread_join() and it being able to handle more than two threads. For every 
thread that is created we yield the thread after joining has been called. Then 
bring the correct order of such threads and stop.

3. test_preempt.c: For this test program, we create a function that takes a 
long time to execute and a function that will take a short time to execute. 
Without preempt, if we create a thread for the long function first, then the 
short function will have to wait till the long function finishes. With preempt 
enabled, the short function will be able to execute almost immediately before 
the long function finishes. 

## References
1. Creating Static Library: 
https://stackoverflow.com/questions/31421616/c-creating-static-library-and-linking-using-a-makefile/31421842
2. Flag Options: 
https://www.gnu.org/software/make/manual/html_node/Options-Summary.html
3. Makefile Tutorial: https://makefiletutorial.com/
4. Makefile Automatic Variables: 
https://stackoverflow.com/questions/3220277/what-do-the-makefile-symbols-and-mean
5. setitimer(): 
https://stackoverflow.com/questions/35666008/the-use-of-getitimer-and-setitimer-in-c
6. sigemptyset(): 
https://www.ibm.com/docs/en/zos/2.2.0?topic=functions-sigemptyset-initialize-signal-mask-exclude-all-signals
7. queue: 
https://www.geeksforgeeks.org/queue-set-1introduction-and-array-implementation/
### Authors
Created by Jeffery Zhang and Sergio Santoyo
