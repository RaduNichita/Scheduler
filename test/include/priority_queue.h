

#ifndef __PRIORITY_QUEUE_H_
#define __PRIORITY_QUEUE_H_
#include "vector.h"

#define PARENT(x) ((x - 1) / 2)
#define LEFT_SON(x) (2 * x + 1)
#define RIGHT_SON(x) (2 * x + 2)


typedef int (*comparator_t)(const void *, const void *);
typedef void (*copy_t)(void *, void *, size_t);

typedef struct {
	vector_t *container;
	comparator_t comp;
} priority_queue_t;


/**
 * Initialize a priority queue data structure.
 * data_size = length of a priority element
 * comp = comparator function used to compare keys
 * copy = copy function used to move elements when inserting
 * @return = priority_queue after initialization
 */
priority_queue_t *priority_queue_init(size_t data_size,
			comparator_t comp, copy_t copy);

/**
 * Return the size of the priority queue
 * pq = priority queue
 * @return = priority_queue datasize
 */
size_t priority_queue_size(priority_queue_t *pq);

/**
 * Check whether the priority queue is empty or not.
 * pq = priority queue pq
 * @return = 1 if the priority queue is empty, 0 otherwise
 */
int priority_queue_empty(priority_queue_t *pq);

/**
 * Add an element into the priority queue.
 * pq = priority queue pq
 * data = data to be inserted in priority queue
 */
void priority_queue_push(priority_queue_t *pq, void *data);

/**
 * Removes the front element from the priority queue;
 * pq = priority queue pq.
 */
void priority_queue_pop(priority_queue_t *pq);

/**
 * Retrieves the front element from the priority queue;
 * pq = priority queue pq.
 * @return the top element from the priority queue
 */
void *priority_queue_top(priority_queue_t *pq);

/**
 * Frees the data alocated from the priority queue;
 * pq = priority queue pq;
 */
void priority_queue_free(priority_queue_t *pq);

#endif /* __PRIORITY_QUEUE_H_ */
