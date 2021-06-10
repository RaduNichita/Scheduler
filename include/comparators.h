/* Copyright Radu Nichita radunichita99@gmail.com */
#ifndef __COMPARATORS_H_
#define __COMPARATORS_H_

#include "so_scheduler.h"
#include "utils.h"

/** compare two ints similar to std::less operator.
 * first = first number as int
 * second = second number as int
 * @return first < second
 */
int compare_ints_less(const void *first, const void *second);

/** compare two ints simillar to std::greater operator.
 * first = first number as int
 * second = second number as int
 * @return first > second
 */
int compare_ints_greater(const void *first, const void *second);

/** compare two so_threads.
 * sort them after priority descending and after timestamp ascending
 * first = pointer to a so_thread structure
 * second = pointer to the other so_thread structure
 * @return >0 if first > second and <0, otherwise.
 */
int compare_so_threads(const void *first, const void *second);

#endif /*__COMPARATORS_H_ */
