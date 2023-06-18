#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include "stdlib.h"

typedef struct RingBuffer {
    float* items;
    size_t head;
    size_t tail;
    int is_full;
    int is_empty;
    size_t size;
} RingBuffer;

RingBuffer ring_buffer_init(size_t buffer_size);
void ring_buffer_reset(RingBuffer* me);
void ring_buffer_write(RingBuffer* buffer, float* data, size_t count);
int ring_buffer_read(RingBuffer* buffer, float* output, size_t count);
size_t ring_buffer_size(RingBuffer* buffer);
void ring_buffer_print(RingBuffer* me);

#endif
