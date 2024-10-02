#ifndef UTILS_INCLUDED
#define UTILS_INCLUDED

#include <stdio.h>
#include <stdlib.h>

#define FOREVER for(;;)
typedef enum {FALSE, TRUE} bool;

void* check_memory_alloc(void* allocation);

#endif

