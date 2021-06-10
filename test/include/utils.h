/**
 * SO, 2019
 * Lab #8, Thread-uri
 *
 * Useful structures/macros
 */

#ifndef UTILS_H_
#define UTILS_H_	1

#include <stdio.h>
#include <stdlib.h>

/* useful macro for handling error codes */
#define DIE(assertion, call_description)				\
	do {								\
		if (assertion) {					\
			fprintf(stderr, "(%s, %d): ",			\
					__FILE__, __LINE__);		\
			perror(call_description);			\
			exit(EXIT_FAILURE);				\
		}							\
	} while (0)

/** duplicate a string, perform similar to strdup
 * src = string to be duplicated
 * @return a duplicated string of src. Memory should be freed by caller.
 */
char *strduplicate(const char *src);

#endif
