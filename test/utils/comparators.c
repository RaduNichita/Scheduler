#include "comparators.h"

int compare_ints_less(const void *first, const void *second)
{
	if (!first || !second)
		return 0;
	return (*(int *)first) - (*(int *)second);
}

int compare_ints_greater(const void *first, const void *second)
{
	if (!first || !second)
		return 0;
	return (*(int *)second) - (*(int *)first);
}

int compare_so_threads(const void *first, const void *second)
{
	so_thread_t **first_th, **second_th;
	int timestamp_first, timestamp_second, priority_first, priority_second;

	first_th = (so_thread_t **)first;
	second_th = (so_thread_t **)second;

	DIE(first == NULL || second == NULL, "compare failed");

	timestamp_first = (*first_th)->thread_timestamp;
	timestamp_second = (*second_th)->thread_timestamp;

	priority_first = (*first_th)->arg.priority;
	priority_second = (*second_th)->arg.priority;

	if (priority_first != priority_second)
		return priority_second - priority_first;
	else
		return timestamp_first - timestamp_second;
}

