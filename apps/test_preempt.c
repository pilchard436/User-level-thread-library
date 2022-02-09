#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <queue.h>


#define TEST_ASSERT(assert)				\
do {									\
	printf("ASSERT: " #assert " ... ");	\
	if (assert) {						\
		printf("PASS\n");				\
	} else	{							\
		printf("FAIL\n");				\
		exit(1);						\
	}									\
} while(0)

void test_preempt(void){

}


int main(void)
{
	test_preempt();

	return 0;
}
