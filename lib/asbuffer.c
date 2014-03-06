#include <assert.h>
#include <sys/types.h>
#include <stdio.h>
#include "asbuffer.h"
#include "zmalloc.h"

asBuffer* asBufferCreate(size_t initSize) {
	asBuffer* b = NULL;

	assert(initSize > 0);

	b = (asBuffer*)zcalloc(sizeof(*b));
	if(b == NULL) {
		return NULL;
	}

	b->start = (uint8_t*)zcalloc(initSize);
	if(b->start == NULL) {
		zfree(b);
		return NULL;
	}

	b->pos = b->start;
	b->last = b->pos;
	b->end = b->start + initSize;

	return b;
}

void asBufferReset(asBuffer* b) {
	assert(b != NULL && b->pos != NULL);

	b->pos = b->start;
	b->last = b->start;
}

void asBufferDestroy(asBuffer* b) {
	assert(b != NULL && b->start != NULL);

	zfree(b->start);
	zfree(b);
}
