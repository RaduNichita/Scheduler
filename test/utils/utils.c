
#include <stdio.h>
#include <string.h>

#include "utils.h"

/* the function performs simillar to strdup */
char *strduplicate(const char *src)
{
	int l = strlen(src) + 1;
	char *dest;

	if (!src)
		return NULL;

	dest = malloc(l * sizeof(char));
	if (!dest)
		return dest;

	memcpy(dest, src, l);
	return dest;
}
