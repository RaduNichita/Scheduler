#include <stdlib.h>
#include <string.h>
#include "vector.h"

/* default copy function for moving elements across the vector */
static void default_copy_elem(void *new_elem, void *old_elem, size_t data_size)
{
	memcpy(new_elem, old_elem, data_size);
}

/* initialize a vector */
vector_t *vector_init(size_t data_size)
{
	vector_t *v = malloc(sizeof(vector_t));

	if (!v)
		return NULL;
	v->capacity = INITIAL_CAPACITY;
	v->size = 0;
	v->data_size = data_size;
	v->data = malloc(INITIAL_CAPACITY * data_size);

	if (!v->data) {
		free(v);
		v = NULL;
		return v;
	}
	v->copy_element = default_copy_elem;

	return v;
}

/* change the copy function for a vector */
void vector_copy_function(vector_t *v,
		void (*copy_element)(void *, void *, size_t))
{
	v->copy_element = copy_element;
}

/* does the reallocation of the vector - EXPAND / SHRINK */
static int vector_alloc(vector_t *v, size_t new_size)
{
	void *old_data;

	if (!v)
		return -1;

	old_data = realloc(v->data, new_size);
	if (!old_data)
		return -1;
	v->data = old_data;
	return 0;
}

/* insert an element at the end of the vector and resize if necessary */
int vector_push_back(vector_t *v, void *data)
{
	int res;

	if (!data || !v)
		return -1;

	if (v->size == (int)(v->capacity * GROWTH_RESIZE_FACTOR)) {
		res = vector_alloc(
			v,
			((int)(v->capacity * GROWTH_FACTOR)) * v->data_size);
		if (res == -1)
			return -1;
		v->capacity = v->capacity * GROWTH_FACTOR;
	}
	v->copy_element((char *) v->data +
			(v->size * v->data_size),
			(char *) data, v->data_size);
	v->size++;
	return 0;
}

/* remove the last element from vector and resize if necessary */
int vector_pop_back(vector_t *v)
{
	int res;

	if (!v || !v->size)
		return -1;

	if (v->size >= INITIAL_CAPACITY &&
		v->size == (int)(v->capacity * SHRINK_RESIZE_FACTOR)) {
		res = vector_alloc(
			v,
			((int)(v->capacity * SHRINK_FACTOR)) * v->data_size);
		if (res == -1)
			return -1;
		v->capacity = (size_t) (v->capacity * SHRINK_FACTOR);
	}
	v->size--;
	return 0;
}

/* get the element from the vector at given index */
void *vector_get(vector_t *v, size_t index)
{
	if (!v || index > v->size)
		return NULL;

	return (char *) v->data + index * v->data_size;
}

/* get the last element from vector */
void *vector_get_back(vector_t *v)
{
	if (!v || !v->size)
		return NULL;
	return vector_get(v, v->size - 1);
}

/* get first element from vector */
void *vector_get_front(vector_t *v)
{
	if (!v || !v->size)
		return NULL;
	return vector_get(v, 0);
}

/* checks whether the vector is empty or not */
int vector_empty(vector_t *v)
{
	if (!v)
		return -1;
	if (!v->size)
		return 1;
	return 0;
}

/* return the datatype stored in vector */
size_t vector_data_size(vector_t *v)
{
	if (!v)
		return -1;
	return v->data_size;
}

/* return the number of the elements stored in vector */
size_t vector_size(vector_t *v)
{
	if (!v)
		return -1;
	return v->size;
}

/* return the capacity of the vector */
size_t vector_capacity(vector_t *v)
{
	if (!v)
		return -1;
	return v->capacity;
}

/* resize the vector to not blow way space */
void vector_shrink_to_fit(vector_t *v)
{
	if (!v)
		return;
	vector_alloc(v, v->size * v->data_size);
	v->capacity = v->size;
}

/* free the resources allocated for a vector */
void free_vector(vector_t *v)
{
	free(v->data);
	free(v);
}

/* get iterator to specific position in vector */
static iterator_t *get_iterator(vector_t *v, size_t index)
{
	iterator_t *it = malloc(sizeof(iterator_t));

	if (!it)
		return NULL;

	it->pointer = NULL;
	it->data_size = 0;

	if (!v || !v->size || index > v->size)
		return it;

	it->pointer = (char *) v->data + index * v->data_size;
	it->data_size = v->data_size;
	return it;
}

/* get vector iterator to specific position in vector */
vector_iterator_t *vector_iterator(vector_t *v, size_t index)
{
	vector_iterator_t *it = malloc(sizeof(vector_iterator_t));

	if (!it)
		return NULL;

	it->begin_iter = get_iterator(v, 0);
	it->current_iter = get_iterator(v, index);
	it->end_iter = get_iterator(v, v->size);
	return it;
}

/* get vector iterator to begin of the vector */
vector_iterator_t *vector_begin(vector_t *v)
{
	return vector_iterator(v, 0);
}

/* get vector iterator to end of the vector */
vector_iterator_t *vector_end(vector_t *v)
{
	return vector_iterator(v, v->size);
}

/* moves an iterator to next position */
static void next_iterator(iterator_t *it)
{
	if (!it)
		return;

	it->pointer = (char *) it->pointer + it->data_size;
}

/* moves an iterator to previous position */
static void prev_iterator(iterator_t *it)
{
	if (!it)
		return;

	it->pointer = (char *) it->pointer - it->data_size;
}

/* moves a vector iterator to the next position */
void vector_next_iterator(vector_iterator_t *it)
{
	if (!it)
		return;

	next_iterator(it->current_iter);
}

/* moves a vector iterator to the previous position */
void vector_prev_iterator(vector_iterator_t *it)
{
	if (!it)
		return;

	prev_iterator(it->current_iter);
}

/* check if iterator has reached end of the vector */
int vector_reached_end(vector_iterator_t *it)
{
	if (!it)
		return -1;
	if (it->current_iter->pointer == it->end_iter->pointer)
		return 0;
	return 1;
}

/* check if iterator has reached begin of the vector */
int vector_reached_begin(vector_iterator_t *it)
{
	if (!it)
		return -1;

	if (it->current_iter->pointer == it->end_iter->pointer)
		vector_prev_iterator(it);

	if ((char *) it->current_iter->pointer ==
			(char *) it->begin_iter->pointer -
						it->begin_iter->data_size)
		return 0;

	return 1;
}

/* return the value stored in iterator */
static void *iterator_get_value(iterator_t *it)
{
	if (!it)
		return NULL;

	return it->pointer;
}

/* return the value stored in vector iterator */
void *vector_iterator_get_value(vector_iterator_t *it)
{
	if (!it)
		return NULL;

	return iterator_get_value(it->current_iter);
}

/* free a vector iterator */
void free_vector_iterator(vector_iterator_t *it)
{
	free(it->begin_iter);
	free(it->current_iter);
	free(it->end_iter);
	free(it);
}
