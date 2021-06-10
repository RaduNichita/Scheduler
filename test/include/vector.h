
#ifndef __VECTOR_H_
#define __VECTOR_H_

#include <stdio.h>

#define INITIAL_CAPACITY 10
#define GROWTH_RESIZE_FACTOR 0.75
#define GROWTH_FACTOR 2
#define SHRINK_RESIZE_FACTOR 0.25
#define SHRINK_FACTOR 0.5


/** structure used to mimic generic iterator from C++.
 * pointer = pointer to value from iterator
 * size_t data_size = data_size of element stored in iterator
 */
typedef struct {
	void *pointer;
	size_t data_size;
} iterator_t;

/** structure used to mimic std::vector<T>::iterator (a bidirectional iterator)
 * current_iter = current iterator in vector
 * begin_iter = constant iterator to first iterator in vector
 * end_iter = constant iterator to last iterator in vector
 */
typedef struct {
	iterator_t *current_iter;
	iterator_t *begin_iter;
	iterator_t *end_iter;
} vector_iterator_t;

/** structured used to mimic std::vector from C++.
 * data = pointer to values from the vector
 * data_size = number of bytes to store an element
 * size = number of elements actually stored in vector.
 * capacity = max number of elements until next resize
 * copy_element = how elements should be moved across vector.
 */
typedef struct {
	void *data;
	size_t data_size;
	size_t size;
	size_t capacity;
	void (*copy_element)(void *new, void *old, size_t size);
} vector_t;

/**
 * Initialize a vector data structure.
 * data_size = length of a vector element
 * @return = vector after initializion
 */
vector_t *vector_init(size_t data_size);

/**
 * Add an element in vector on last position and resize if necessary
 * v = vector
 * data = data to be added in vector
 * @return 1 if could resize, 0 otherwise
 */
int vector_push_back(vector_t *v, void *data);

/**
 * Remove an element in vector from last position and resize if necessary
 * v = vector
 * @return 1 if could resize, 0 othewise
 */
int vector_pop_back(vector_t *v);

/**
 * Get an element from the vector. Similar to v[i] <=> get(v, i)
 * v = vector
 * index = position
 * @return the element on the ith element.
 */
void *vector_get(vector_t *v, size_t index);

/**
 * Get the element on the last position. Similar to back() from std::vector
 * v = vector
 * @return the last element from the vector or NULL if doesn't exist
 */
void *vector_get_back(vector_t *v);

/**
 * Get the element on the first position. Similar to front() from std::vector
 * v = vector
 * @return the first element from the vector or NULL if doesn't exist
 */
void *vector_get_front(vector_t *v);

/**
 * Checks whether the vector is empty or not.
 * v = vector
 * @return 1 if the vector is empty, 0 otherwise.
 */
int vector_empty(vector_t *v);

/**
 * Return the size of the vector v.
 * v = vector
 * @return the number of element that v has.
 */
size_t vector_size(vector_t *v);

/**
 * Get the maximum capacity of v before the next resize.
 * v = vector
 * @return the current capacity of v
 */
size_t vector_capacity(vector_t *v);

/**
 * Resize the vector to match v.size = v.capacty
 * v = vector
 */
void vector_shrink_to_fit(vector_t *v);

/**
 * Get the data_size of elements stored in vector v.
 * v = vector
 * @return the datasize of element stored in v.
 */
size_t vector_data_size(vector_t *v);

/**
 * Free the resource allocated for vector v.
 * v = vector
 */
void free_vector(vector_t *v);

/**
 * Get the iterator at ith position. Similar to v.begin() + index;
 * v = vector
 * @return iterator to ith position in vector.
 */
vector_iterator_t *vector_iterator(vector_t *v, size_t index);

/**
 * Get the iterator at the beginning of the vector. Similiar to v.begin()
 * v = vector
 * @return iterator to begin position in vector.
 */
vector_iterator_t *vector_begin(vector_t *v);

/** Get the iterator at the ending of the vector. Similar to v.end()
 * v = vector
 * @return iterator to end position in vector.
 */
vector_iterator_t *vector_end(vector_t *v);

/** Move the iterator to the next position in the vector.
 * v = vector iterator
 * @return none, since it modifies the current iterator
 */
void vector_next_iterator(vector_iterator_t *it);

/** Move the iterator to the previous position in the vector.
 * it = vector iterator
 * @return none, since it modifies the current iteratior
 */
void vector_prev_iterator(vector_iterator_t *it);

/** Checks whether the current iterator has reached the end of the vector v
 * it = vector iterator
 * @return 1 if iterator has reached end, 0 otherwise.
 */
int vector_reached_end(vector_iterator_t *it);

/** Checks whether the current iterator has reached the begin of the vector v.
 * it = vector iterator
 * @return 1 if iterator has reached begin, 0 otherwise.
 */
int vector_reached_begin(vector_iterator_t *it);


/** Specifies how elements should be moved through the vector v.
 * v = vector
 * copy_element = function with 3 parameters
 * @first_param = the address of the destination
 * @second_param = the address of the source
 * @third_param = data_size of the element.
 */
void vector_copy_function(vector_t *v,
			void (*copy_element)(void *, void *, size_t));

/** Get the actual value stored in iterator it.
 * it = vector_iterator
 * return the data stored in iterator it or NULL if reached end or begin.
 */
void *vector_iterator_get_value(vector_iterator_t *it);

/** Free a vector iterator.
 * it = vector iterator
 */
void free_vector_iterator(vector_iterator_t *it);

#endif /* __VECTOR_H_ */
