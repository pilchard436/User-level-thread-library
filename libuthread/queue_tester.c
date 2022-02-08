
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
    int data[5] = {1,2,3,4,5};
	
	int *ptr1, *ptr2, *ptr3, *ptr4, *ptr5;

	// for (int i = 0; i < 5; i++){
	// 	data[i] = (int *)malloc(sizeof(int));
	// 	data[i] = i+1;
    //     printf(" %d is data \n",data[i]);
	// }
	queue_t q;

	fprintf(stderr, "*** TEST queue_complex ***\n");

	q = queue_create();
	queue_enqueue(q, (void*) &data[0]);
    queue_enqueue(q, (void*) &data[1]);
    queue_dequeue(q, (void**) &ptr1);
    queue_dequeue(q, (void**)&ptr2);
    queue_enqueue(q, (void*) &data[2]);
    queue_enqueue(q, (void*) &data[3]);
    queue_dequeue(q, (void**)&ptr3);
    queue_enqueue(q, (void*) &data[4]);
	queue_dequeue(q, (void**)&ptr4);
    queue_dequeue(q, (void**)&ptr5);

	TEST_ASSERT(*ptr1 == 1);
    TEST_ASSERT(*ptr2 == 2);
    TEST_ASSERT(*ptr3 == 3);
    TEST_ASSERT(*ptr4 == 4);
    TEST_ASSERT(*ptr5 == 5);
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
    //test_dequeue_nonempty_data();
    test_queue_complex();
    test_delete();
    test_delete_empty();
    test_delete_empty_data();
    test_delete_no_match();
    test_iterator_empty();
	test_iterator();

	return 0;
}