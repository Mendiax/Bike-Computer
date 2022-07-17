#ifndef __RINGBUFFER_H__
#define __RINGBUFFER_H__

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

#ifdef __cplusplus
 extern "C" {
#endif

typedef struct RingBuffer
{
    size_t size_of_element;
    size_t max_queue_length;
    size_t current_index;
    size_t current_queue_length;
    char *min;
    char *max;
    char *avg;
    char data_pointer[];
} RingBuffer;


RingBuffer *ring_buffer_create_static(uint8_t allocatedMemory[],  size_t size_of_element, size_t buffer_length);

#define RING_BUFFER_CREATE(size_of_element, buffer_length) ( \
    uint8_t allocatedMemory[size_of_element * buffer_length], \
    ring_buffer_create_static(allocatedMemory,size_of_element,buffer_length)\
    )


RingBuffer *ring_buffer_create(size_t size_of_element, size_t buffer_length);

void ring_buffer_destroy(RingBuffer *ring_buffer);

/*
 * returns 1 if queue is full
 */
int ring_buffer_is_full(RingBuffer *ring_buffer);

/*
 * returns 1 if queue is empty
 */
int ring_buffer_is_empty(RingBuffer *ring_buffer);

char *ring_buffer_get_element_pointer(RingBuffer *ring_buffer, size_t index);

#define ring_buffer_get_element(ring_buffer, index, type) (*((type *)ring_buffer_get_element_pointer(ring_buffer, index)))

#define ring_buffer_get_element_at(ring_buffer, index, type) (*((type *)ring_buffer_get_element_pointer(ring_buffer, (index + ring_buffer->current_index) % ring_buffer->max_queue_length)))

float ring_buffer_get_element_at_float(RingBuffer *ring_buffer, int index);

uint8_t ring_buffer_get_element_at_byte(RingBuffer *ring_buffer, int index);

uint8_t ring_buffer_get_last_element(RingBuffer *ring_buffer);

/* for uint8_t only*/
uint8_t ring_buffer_get_max(RingBuffer *ring_buffer);

/* for uint8_t only*/
uint8_t ring_buffer_get_min(RingBuffer *ring_buffer);

char *ring_buffer_get_last_element_pointer(RingBuffer *ring_buffer);

void ring_buffer_inc_index(RingBuffer *queue);

int ring_buffer_pop(RingBuffer *ring_buffer, char *new_element);

int ring_buffer_push(RingBuffer *ring_buffer, const char *element);

int ring_buffer_push_overwrite(RingBuffer *ring_buffer, const char *element);

#ifdef __cplusplus
}
#endif

#endif
