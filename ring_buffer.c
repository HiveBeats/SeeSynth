#include "ring_buffer.h"
#include "utils.h"

RingBuffer ring_buffer_init(size_t buffer_size) {
    RingBuffer buffer = {
        .items = calloc(buffer_size, sizeof(float)),
        .head = 0,
        .tail = 0,
        .is_full = 0,
        .is_empty = 1,
        .size = buffer_size
    };

    return buffer;
}

void ring_buffer_reset(RingBuffer* me) {
    me->head = 0;
    me->tail = 0;
    me->is_full = 0;
}

// +
static void advance_pointer(RingBuffer* me) {
    if(me->is_full) {
        me->tail++;
		if (me->tail == me->size) { 
			me->tail = 0;
		}
	}
    me->head++;
	if (me->head == me->size) { 
		me->head = 0;
	}
    size_t is_full = me->head == me->tail ? 1 : 0;
	me->is_full = is_full;
}

// -
static void retreat_pointer(RingBuffer* me) {
	me->is_full = 0;
    me->tail++;
	if (me->tail == me->size) { 
		me->tail = 0;
	}
}

void ring_buffer_write(RingBuffer* buffer, float* data, size_t count) {
    if (buffer->is_full || buffer->head + count > buffer->size) {
        write_log("[WARN] Trying to overfill the ring buffer: \n\tIsFull:%d\n\tHead:%zu\n\tCount:%zu\n\t", 
            buffer->is_full,
            buffer->head, 
            count);
        return;
    }
    buffer->is_empty = 0;

    for (size_t i = 0; i < count; i++) {
        buffer->items[buffer->head] = data[i];
        advance_pointer(buffer);
    }
    //me->is_empty = is_full && (me->head == me->tail);
}

int ring_buffer_read(RingBuffer* buffer, float* output, size_t count) {
    if (buffer->is_empty) {
        write_log("[WARN] Trying to read empty buffer");
        return 0;
    }

    for (size_t i = 0; i < count; i++) {
        output[i] = buffer->items[buffer->tail];
        retreat_pointer(buffer);
    }
    buffer->is_empty = !buffer->is_full && (buffer->head == buffer->tail);
    return 1;
}

size_t ring_buffer_size(RingBuffer* buffer) {
	size_t size = buffer->size;
	if(!buffer->is_full) {
		if(buffer->head >= buffer->tail) {
			size = (buffer->head - buffer->tail);
		}
		else {
			size = (buffer->size + buffer->head - buffer->tail);
		}
	}

	return size;
}

void ring_buffer_print(RingBuffer* me) {
    write_log("[INFO] The ring buffer: \n\tIsFull:%d\n\tIsEmpty:%d\n\tHead:%zu\n\tTail:%zu\n\t", 
                me->is_full, 
                me->is_empty, 
                me->head, 
                me->tail);
}
