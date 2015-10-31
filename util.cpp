#include <stdlib.h>

void *xmalloc(int size)
{
	void *data;

	data = malloc(size);

	if (data == NULL)
		exit(1);
	
	return data;
}

void *xrealloc(void *ptr, int size)
{
	void *ret;

	ret = realloc(ptr, size);

	if (ret == NULL)
		exit(1);
	
	return ret;
}
