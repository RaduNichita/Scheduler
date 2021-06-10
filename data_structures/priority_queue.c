
/* Copyright Radu Nichita radunichita99@gmail.com */
#include <stdlib.h>
#include <string.h>

#include "priority_queue.h"
#include "utils.h"

/* initialize a priority queue */
priority_queue_t *priority_queue_init(size_t data_size, comparator_t comp, copy_t copy)
{
	priority_queue_t *pq;
	vector_t *v;

	pq = malloc(sizeof(priority_queue_t));
	if (!pq)
		return NULL;

	v = vector_init(data_size);
	if (!v) {
		free(pq);
		return NULL;
	}

	/* if a copy function is given, assign it */
	if (copy != NULL)
		vector_copy_function(v, copy);
	pq->container = v;
	pq->comp = comp;
	return pq;
}

/* get the size of the priority_queue */
size_t priority_queue_size(priority_queue_t *pq)
{
	if (!pq)
		return -1;
	return pq->container->size;
}

/* checks if the priority queue is empty */
int priority_queue_empty(priority_queue_t *pq)
{
	if (!pq)
		return -1;
	return vector_empty(pq->container);
}

/* swaps two elements from the pq */
static void swap(void *a, void *b, int data_size,
			void (*move_elem)(void *, void *, size_t))
{
	void *tmp = malloc(data_size);

	move_elem(tmp, a, data_size);
	move_elem(a, b, data_size);
	move_elem(b, tmp, data_size);
	free(tmp);
}

/* reestablish the heap order after push */
static void heapify_up(priority_queue_t *pq, size_t index)
{
	vector_t *v;
	comparator_t comp;

	if (!pq || !pq->comp)
		return;

	v = pq->container;
	comp = pq->comp;

	while (index > 0) {

		void *parent = vector_get(v, PARENT(index));
		void *current = vector_get(v, index);

		if (comp(parent, current) >= 0)
			swap(parent, current,
				v->data_size, pq->container->copy_element);
		index = PARENT(index);
	}
}

/* reestablish heap order after pop */
static void heapify_down(priority_queue_t *pq, size_t index)
{
	vector_t *v;
	comparator_t comp;
	size_t left_son, right_son, min_son;
	void *current, *left, *right;

	v = pq->container;
	comp = pq->comp;

	if (!pq || !pq->comp)
		return;

	while (index < vector_size(v)) {
		min_son = index;
		left_son = LEFT_SON(index);
		right_son = RIGHT_SON(index);
		current = vector_get(v, index);

		if (left_son < vector_size(v)) {
			left = vector_get(v, left_son);
			if (comp(left, current) <= 0)
				min_son = left_son;
		}

		if (right_son < vector_size(v)) {
			right = vector_get(v, right_son);
			if (comp(right, left) <= 0)
				min_son = right_son;
		}

		if (min_son == index)
			break;
		swap(current, vector_get(v, min_son),
					v->data_size, v->copy_element);
		index = min_son;
	}
}

/* adds an element to the priority queue */
void priority_queue_push(priority_queue_t *pq, void *data)
{
	if (!pq || !data)
		return;
	vector_push_back(pq->container, data);
	heapify_up(pq, vector_size(pq->container) - 1);

}

/* removes an element from the priority queue */
void priority_queue_pop(priority_queue_t *pq)
{

	vector_t *v;

	if (!pq || !pq->container->size)
		return;

	v = pq->container;

	if (priority_queue_size(pq) > 1)
		swap(vector_get_front(v), vector_get_back(v),
				v->data_size, v->copy_element);

	vector_pop_back(pq->container);
	if (priority_queue_size(pq) > 1)
		heapify_down(pq, 0);
}

/* get the address of the top element of the pq */
void *priority_queue_top(priority_queue_t *pq)
{
	if (!pq)
		return NULL;
	return vector_get_front(pq->container);
}

/* free the resources allocated for the pq */
void priority_queue_free(priority_queue_t *pq)
{
	if (!pq)
		return;
	free_vector(pq->container);
	free(pq);
}
