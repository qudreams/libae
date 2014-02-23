/*
 *a simple buffer implementation for network connection.
 */

#ifndef asBUFFER_H
#define asBUFFER_H

#include <stdint.h>

//caculate the capacity of buffer
#define asBufferCap(b) (size_t)(b->end - b->start)
//caculate the length of data that we have not process in buffer.
#define asBufferLen(b) (size_t)(b->last - b->pos)

typedef struct {
	uint8_t* start;
	uint8_t* end;
	uint8_t* pos;
	uint8_t* last;
} asBuffer;

asBuffer* asBufferCreate(size_t initSize);
void asBufferReset(asBuffer* b);
void asBufferDestroy(asBuffer* b);

#endif