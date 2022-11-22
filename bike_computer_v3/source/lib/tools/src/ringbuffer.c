#include "ringbuffer.h"
#include <string.h>


// static

static void ring_buffer_inc_index(Ring_Buffer *queue);

 #define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })



// public

Ring_Buffer *ring_buffer_create(size_t size_of_element, size_t buffer_length)
{
    Ring_Buffer *ring_buffer = (Ring_Buffer *)calloc(sizeof(Ring_Buffer) + size_of_element * buffer_length, 1);
    if (!ring_buffer)
    {
        return (Ring_Buffer *)0;
    }
    ring_buffer->min = (char *)calloc(1, size_of_element);
    ring_buffer->max = (char *)calloc(1, size_of_element);
    ring_buffer->avg = (char *)calloc(1, size_of_element);
    ring_buffer->max_queue_length = buffer_length;
    ring_buffer->size_of_element = size_of_element;
    ring_buffer->current_queue_length = 0;
    ring_buffer->current_index = 0;
    return ring_buffer;
}

void ring_buffer_destroy(Ring_Buffer *ring_buffer)
{
    free(ring_buffer);
}


/*
 * returns 1 if queue is full
 */
int ring_buffer_is_full(Ring_Buffer *ring_buffer)
{
    return ring_buffer->current_queue_length == ring_buffer->max_queue_length;
}

/*
 * returns 1 if queue is empty
 */
int ring_buffer_is_empty(Ring_Buffer *ring_buffer)
{
    return ring_buffer->current_queue_length == (size_t)0;
}

char *ring_buffer_get_element_pointer(Ring_Buffer *ring_buffer, size_t index)
{
    return ring_buffer->data_pointer + ring_buffer->size_of_element * index;
}

#define ring_buffer_get_element(ring_buffer, index, type) (*((type *)ring_buffer_get_element_pointer(ring_buffer, index)))

#define ring_buffer_get_element_at(ring_buffer, index, type) (*((type *)ring_buffer_get_element_pointer(ring_buffer, (index + ring_buffer->current_index) % ring_buffer->max_queue_length)))


uint8_t ring_buffer_get_last_element(Ring_Buffer *ring_buffer)
{
    if (ring_buffer_is_empty(ring_buffer))
    {
        return 0;
    }
    return ring_buffer_get_element_at_byte(ring_buffer, ring_buffer->current_queue_length - 1);
}

char *ring_buffer_get_last_element_pointer(Ring_Buffer *ring_buffer)
{
    if (ring_buffer_is_empty(ring_buffer))
    {
        return ring_buffer->data_pointer;
    }
    size_t nextFree = (ring_buffer->current_index + ring_buffer->current_queue_length - 1) % ring_buffer->max_queue_length;
    return ring_buffer_get_element_pointer(ring_buffer, nextFree);
}

int ring_buffer_pop(Ring_Buffer *ring_buffer, char *new_element)
{
    if (ring_buffer_is_empty(ring_buffer))
    {
        return 0;
    }
    memcpy(new_element, ring_buffer_get_element_pointer(ring_buffer, ring_buffer->current_index), ring_buffer->size_of_element);
    ring_buffer_inc_index(ring_buffer);
    ring_buffer->current_queue_length--;
    return 1;
}

int ring_buffer_push(Ring_Buffer *ring_buffer, const char *element)
{
    if (ring_buffer_is_full(ring_buffer))
    {
        return -1;
    }
    if (!ring_buffer->current_queue_length)
    {
        memcpy(ring_buffer_get_element_pointer(ring_buffer, ring_buffer->current_index), element, ring_buffer->size_of_element);
    }
    else
    {
        size_t nextFree = (ring_buffer->current_index + ring_buffer->current_queue_length) % ring_buffer->max_queue_length;
        memcpy(ring_buffer_get_element_pointer(ring_buffer, nextFree), element, ring_buffer->size_of_element);
    }
    ring_buffer->current_queue_length++;
    return 0;
}












static void ring_buffer_inc_index(Ring_Buffer *queue)
{
    queue->current_index++;
    queue->current_index = queue->current_index % queue->max_queue_length;
}
