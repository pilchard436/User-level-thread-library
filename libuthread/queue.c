#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

#define DEFAULT_QUEUE_SIZE 10

struct queue {
	int head, tail, size, capacity;
	void **arr;
};

int queue_empty(queue_t queue){
	return (queue == NULL || queue->arr[queue->head] == NULL || 
	(queue->size == 0) || (queue->tail == queue->head - 1));
}

queue_t queue_create(void)
{
	//allocating memory for the size of the queue
	queue_t q = (queue_t) malloc(sizeof(queue_t));
	q->head = 0;
	q->capacity = DEFAULT_QUEUE_SIZE;
	q->tail = -1;
	q->size = 0;

	//gathers the address of the ptr
	q->arr = (void**) malloc(DEFAULT_QUEUE_SIZE * sizeof(void*));
	
	if(q->arr != NULL){
		return q;
	}
	return NULL;
}

int queue_destroy(queue_t queue)
{
	//check for not empty queue
	if (queue_empty(queue) == 0){
		return -1;
	}//if empty
	
	//destroy queue and arr
	free(queue);
	return 0;
}

int queue_enqueue(queue_t queue, void *data)
{
	//queue pointer doesn't exist
	if (queue == NULL){
		return -1;
	}
	if (data == NULL){
		return -1;
	}
	//need to check if queue is full
	if (queue->size == queue->capacity){
		// need to realloc arr, if fail return -1
		// if success then enqueue
		queue->capacity += 1;
		queue->arr = realloc(queue->arr, queue->capacity * sizeof(void*));
		if (queue->arr == NULL){
			return -1;
		}
		queue->tail++;
	}
	// queue is not full
	else {
		queue->tail++;
	}
	queue->arr[queue->tail] = data;
	queue->size++;
	return 0;
}

int queue_dequeue(queue_t queue, void **data)
{
	//queue is empty
	if (queue_empty(queue) == 1){
		return -1;
	}
	*data = queue->arr[queue->head];
	if (*data == NULL){
		return -1;
	}
	//free(&(queue->arr[queue->head]));
	//move down the queue to next avaliable
	queue->head++;
	//shrink queue
	queue->size--;
	return 0;
}

int queue_delete(queue_t queue, void *data)
{
	//queue is empty
	if (queue_empty(queue) == 1){
			return -1;
		}	
	//check for no data
	if (data == NULL){
		return -1;
	}

	int count = queue->head;
	void *item = queue->arr[count];

	while (*((int *)item) != *((int *)data) && count != queue->tail){
		count++;
		item = queue->arr[count];
	}
	//reached the end of the queue and did not find item
	if (*((int *)item) != *((int *)data) && count == queue->tail) {
		return -1;
	}
	while (count != queue->tail){
		queue->arr[count] = queue->arr[count + 1];
		count++;
	}
	queue->tail--;
	queue->size--;
	return 0;
}

int queue_iterate(queue_t queue, queue_func_t func, void *arg, void **data)
{

	int count = queue->head;
	int retval;
	
	//queue is empty
	if (queue_empty(queue) == 1){
		return -1;
	}	
	//Traversing from oldest to newest in queue
	while(count != queue->tail){
		retval = (*func)(queue, queue->arr[count], arg);
		if(retval == 1){
			if(data != NULL){
				*data = queue->arr[count];
			}	
			break;
		}
		count++;
	}
	return 0;
}

int queue_length(queue_t queue)
{
	return (queue == NULL) ? -1 : queue->size;
}