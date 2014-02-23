#include "zmalloc.h"
#include <stdio.h>
#include <stdlib.h>


static void zmalloc_default_oom(size_t size) {
    fprintf(stderr, "zmalloc: Out of memory trying to allocate %zu bytes\n",
        size);
    fflush(stderr);
    abort();
}

static void (*zmalloc_oom_handler)(size_t) = zmalloc_default_oom;

void *zmalloc(size_t size) {
    void *ptr = malloc(size);

    if (!ptr) zmalloc_oom_handler(size);
    return ptr;
}
void* zcalloc(size_t size)
{
	void* ptr = calloc(1,size);
	if(!ptr) zmalloc_oom_handler(size);
	return ptr;
}
void* zrealloc(void* ptr,size_t size)
{
	if(ptr == NULL) return zmalloc(size);
	void* newptr = realloc(ptr,size);
	return newptr;
}
void zfree(void *ptr) {
    if (ptr == NULL) return;
    free(ptr);
}
