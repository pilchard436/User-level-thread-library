#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <queue.h>
#include <uthread.h>

int shortfunc1(void){
	printf("This should print after a very long time\n");
	return 0;
}

int shortfunc2(void){
	printf("If this prints immediantly after 'with preempt', preempt works\n");
	return 0;
}

int longfunc(void){
	long i = 0;
	while (i < 10000000000){
		i++;
	}
	printf("%ld\n", i);
	return 0;
}

int main(void)
{
	printf("without preempt\n");
	uthread_start(0);
	int a = uthread_create(longfunc);
	int b = uthread_create(shortfunc1);
    uthread_join(a, NULL);
    uthread_join(b, NULL);
    uthread_stop();

	printf("with preempt\n");
	uthread_start(1);
	a = uthread_create(longfunc);
	b = uthread_create(shortfunc2);
    uthread_join(a, NULL);
    uthread_join(b, NULL);
    uthread_stop();

	return 0;
}
