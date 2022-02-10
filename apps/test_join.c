/*
 * Thread creation and yielding test
 *
 * Tests the creation of multiples threads and the fact that a parent thread
 * should get returned to before its child is executed. The way the printing,
 * thread creation and yielding is done, the program should output:
 *
 * thread3
 * thread2
 * thread1
 * thread0
 */

#include <stdio.h>
#include <stdlib.h>

#include <uthread.h>

int thread3(void)
{
    uthread_yield();
    printf("thread%d\n", uthread_self());
    return 0;
}

int thread2(void)
{
    int retval;
    uthread_join(uthread_create(thread3), &retval);
    printf("thread%d\n", uthread_self());
    printf("retval: %d\n", retval);
    return 0;
}

int thread1(void)
{
    int retval;
    uthread_join(uthread_create(thread2), &retval);
    printf("thread%d\n", uthread_self());
    printf("retval: %d\n", retval);
    uthread_yield();
    return 0;
}

int main(void)
{
    int retval;
    uthread_start(0);
    uthread_join(uthread_create(thread1), &retval);
    printf("thread%d\n", uthread_self());
    printf("retval: %d\n", retval);
    uthread_stop();

    return 0;
}
