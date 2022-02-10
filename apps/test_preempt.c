#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <queue.h>
#include <uthread.h>

int shortfunc(void){
	printf("If this prints immediantly after we execute program, preempt works\n");
	return 0;
}

int longfunc(void){
	sleep(10);
	return 0;
}

int main(void)
{
	uthread_start(1);
	int a = uthread_create(longfunc);
	printf("in main\n");
	int b = uthread_create(shortfunc);
    uthread_join(a, NULL);
    uthread_join(b, NULL);
    uthread_stop();

	return 0;
}
