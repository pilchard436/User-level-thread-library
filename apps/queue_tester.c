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

static int inc_item(queue_t q, void *data, void *arg)
{
    int *a = (int*)data;
    int inc = (int)(long)arg;

    if (*a == 42)
        queue_delete(q, data);
    else
        *a += inc;

    return 0;
}
static int find_item(queue_t q, void *data, void *arg)
{
    int *a = (int*)data;
    int match = (int)(long)arg;
    (void)q; //unused

    if (*a == match)
        return 1;

    return 0;
}

/* Create */
void test_create(void)
{
	fprintf(stderr, "*** TEST create ***\n");

	queue_t q = queue_create();
	TEST_ASSERT(q != NULL);

}

void test_length(void)
{
	fprintf(stderr, "*** TEST length ***\n");

	queue_t q = queue_create();
	int len = queue_length(q);
	TEST_ASSERT(len == 0);
	queue_enqueue(q, (void*) 3);
	queue_enqueue(q, (void*) 5);
	queue_enqueue(q, (void*) 888);
	len = queue_length(q);
	TEST_ASSERT(len == 3);

}

void test_length_empty(void)
{
	fprintf(stderr, "*** TEST length empty ***\n");

	queue_t q = NULL;
	int len = queue_length(q);
	TEST_ASSERT(len == -1);
}

void test_destroy(void)
{
	fprintf(stderr, "*** TEST destroy ***\n");
	queue_t q = queue_create();
    int retval = queue_destroy(q);
	TEST_ASSERT(retval == 0);
}

void test_destroy_nonempty(void)
{
	fprintf(stderr, "*** TEST destroy nonempty ***\n");
    int data = 3;
	queue_t q = queue_create();
    queue_enqueue(q, &data);
    int retval = queue_destroy(q);
	TEST_ASSERT(retval == -1);

}

void test_enqueue(void)
{
	fprintf(stderr, "*** TEST enqueue ***\n");
    int data = 3;
	queue_t q = queue_create();
	int retval = queue_enqueue(q, &data);
    TEST_ASSERT(retval == 0);
}

void test_enqueue_queue_DNE(void)
{
	fprintf(stderr, "*** TEST enqueue queue DNE ***\n");
    int data = 3;
	queue_t q = NULL;
	int retval = queue_enqueue(q, &data);
    TEST_ASSERT(retval == -1);
}

void test_enqueue_data_DNE(void)
{
	fprintf(stderr, "*** TEST enqueue data DNE ***\n");
    int *data = NULL;
	queue_t q = queue_create();
	int retval = queue_enqueue(q, data);
    TEST_ASSERT(retval == -1);
}
void test_dequeue(void)
{
	int *ptr = NULL;
    int data = 3;
	queue_t q;

	fprintf(stderr, "*** TEST dequeue***\n");

	q = queue_create();
    queue_enqueue(q, &data);
	int retval = queue_dequeue(q, (void**)&ptr);
	TEST_ASSERT(retval == 0);
}

void test_dequeue_empty(void)
{
	int *ptr = NULL;
	queue_t q;

	fprintf(stderr, "*** TEST dequeue empty***\n");

	q = queue_create();
	int retval = queue_dequeue(q, (void**)&ptr);
	TEST_ASSERT(retval == -1);
}

/* Enqueue/Dequeue simple */
void test_queue_simple(void)
{
	int data = 3, *ptr;
	queue_t q;

	fprintf(stderr, "*** TEST queue_simple ***\n");

	q = queue_create();
	queue_enqueue(q, &data);
	queue_dequeue(q, (void**)&ptr);
	TEST_ASSERT(ptr == &data);
}

void test_delete(void)
{
	int data = 3;
    int data1 = 3;
	queue_t q;

	fprintf(stderr, "*** TEST delete ***\n");

	q = queue_create();
    queue_enqueue(q, &data1);
	int retval = queue_delete(q, &data);
	TEST_ASSERT(retval == 0);
}

void test_delete_empty(void)
{
	int data = 3;
	queue_t q;

	fprintf(stderr, "*** TEST delete empty***\n");

	q = queue_create();
	int retval = queue_delete(q, &data);
	TEST_ASSERT(retval == -1);
}

void test_delete_empty_data(void)
{
	int *data = NULL;
    int data1 = 3;
	queue_t q;

	fprintf(stderr, "*** TEST delete empty data***\n");

	q = queue_create();
    queue_enqueue(q, &data1);
	int retval = queue_delete(q, data);
	TEST_ASSERT(retval == -1);
}

void test_delete_no_match(void)
{
	int data = 2;
    int data1 = 3;
	queue_t q;

	fprintf(stderr, "*** TEST delete no match***\n");

	q = queue_create();
    queue_enqueue(q, &data1);
	int retval = queue_delete(q, &data);
	TEST_ASSERT(retval == -1);
}

/* Enqueue/Dequeue complex */
void test_queue_complex(void)
{
	int data1 = 3;
    int data[5] = {1,2,3,4,5};
	int *ptr1, *ptr2, *ptr3, *ptr4, *ptr5;

	queue_t q;

	fprintf(stderr, "*** TEST queue_complex ***\n");

	q = queue_create();
	queue_enqueue(q, (void*) &data[0]);
    queue_enqueue(q, (void*) &data[1]);
    queue_dequeue(q, (void**) &ptr1);
    queue_dequeue(q, (void**) &ptr2);
    queue_enqueue(q, (void*) &data[2]);
	queue_delete(q, (void*) &data1);
    queue_enqueue(q, (void*) &data[3]);
    queue_dequeue(q, (void**) &ptr3);
    queue_enqueue(q, (void*) &data[4]);
	queue_dequeue(q, (void**) &ptr4);
    int retval = queue_dequeue(q, (void**) &ptr5);

	TEST_ASSERT(*ptr1 == 1);
    TEST_ASSERT(*ptr2 == 2);
    TEST_ASSERT(*ptr3 == 4);
    TEST_ASSERT(*ptr4 == 5);
    TEST_ASSERT(retval == -1);
}

void test_iterator_empty(void)
{
    queue_t q;

	fprintf(stderr, "*** TEST queue_iterator empty ***\n");


    /* Initialize the queue and enqueue items */
    q = queue_create();

    
    int retval = queue_iterate(q, inc_item, (void*)1, NULL);
    TEST_ASSERT(retval == -1);
}

void test_iterator(void)
{
    queue_t q;
    int data[] = {1, 2, 3, 4, 5, 42, 6, 7, 8, 9};
    size_t i;
    int *ptr;

	fprintf(stderr, "*** TEST queue_iterator ***\n");


    /* Initialize the queue and enqueue items */
    q = queue_create();
    for (i = 0; i < sizeof(data) / sizeof(data[0]); i++)
        queue_enqueue(q, &data[i]);

    /* Add value '1' to every item of the queue, delete item '42' */
    queue_iterate(q, inc_item, (void*)1, NULL);
    TEST_ASSERT(data[0] == 2);
    TEST_ASSERT(queue_length(q) == 9);

    /* Find and get the item which is equal to value '5' */
    ptr = NULL;     // result pointer *must* be reset first
    queue_iterate(q, find_item, (void*)5, (void**)&ptr);
    TEST_ASSERT(ptr != NULL);
    TEST_ASSERT(*ptr == 5);
    TEST_ASSERT(ptr == &data[3]);
}


int main(void)
{
	test_create();
	test_queue_simple();
    test_destroy();
    test_destroy_nonempty();
    test_enqueue();
    test_enqueue_queue_DNE();
    test_enqueue_data_DNE();
    test_dequeue();
    test_dequeue_empty();
    test_queue_complex();
    test_delete();
    test_delete_empty();
    test_delete_empty_data();
    test_delete_no_match();
    test_iterator_empty();
	test_iterator();
	test_length();
	test_length_empty();

	return 0;
}