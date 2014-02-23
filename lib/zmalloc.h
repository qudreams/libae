#ifndef ZMALLOC_H
#define ZMALLOC_H

#include <sys/types.h>

void* zmalloc(size_t size);
void* zcalloc(size_t size);
void* zrealloc(void* ptr,size_t size);
void zfree(void* ptr);

#endif
