#include "utils.h"

/* check memory allocation for dynamic allocation  */
/*if we fail on dynamic allocation we terminate the program*/
void* check_memory_alloc(void* allocation)
{
	if (allocation != NULL)
		return allocation;

	printf("Memory allocation failed. terminating program/n");
	exit(1);
}


