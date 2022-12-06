#ifndef __Ring_Buffer_H__
#define __Ring_Buffer_H__

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

#ifdef __cplusplus
 extern "C" {
#endif

typedef struct Ring_Buffer
{
    size_t size_of_element;
    size_t max_queue_length;
    size_t current_index;
    size_t current_queue_length;
    char *min;
    char *max;
    char *avg;
    char data_pointer[];
} Ring_Buffer;


Ring_Buffer *ring_buffer_create(size_t size_of_element, size_t buffer_length);

void ring_buffer_destroy(Ring_Buffer *ring_buffer);

/*
 * returns 1 if queue is full
 */
int ring_buffer_is_full(Ring_Buffer *ring_buffer);

/*
 * returns 1 if queue is empty
 */
int ring_buffer_is_empty(Ring_Buffer *ring_buffer);

char *ring_buffer_get_element_pointer(Ring_Buffer *ring_buffer, size_t index);

#define ring_buffer_get_element(ring_buffer, index, type) \
    (*((type *)ring_buffer_get_element_pointer(ring_buffer, index)))

#define ring_buffer_get_element_at(ring_buffer, index, type) \
    (*((type *)ring_buffer_get_element_pointer(ring_buffer, (index + ring_buffer->current_index) % ring_buffer->max_queue_length)))

uint8_t ring_buffer_get_last_element(Ring_Buffer *ring_buffer);

char *ring_buffer_get_last_element_pointer(Ring_Buffer *ring_buffer);

int ring_buffer_pop(Ring_Buffer *ring_buffer, char *new_element);

int ring_buffer_push(Ring_Buffer *ring_buffer, const char *element);

int ring_buffer_push_overwrite(Ring_Buffer *ring_buffer, const char *element);

#ifdef __cplusplus
}
#endif

#endif
