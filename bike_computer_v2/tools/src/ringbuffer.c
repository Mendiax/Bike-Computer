#include "../include/ringbuffer.h"
#include <string.h>

 #define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

RingBuffer *ring_buffer_create_static(uint8_t allocatedMemory[],  size_t size_of_element, size_t buffer_length)
{
    RingBuffer *ring_buffer = (RingBuffer *)&allocatedMemory[0];
    ring_buffer->min = (char *)calloc(1, size_of_element);
    ring_buffer->max = (char *)calloc(1, size_of_element);
    ring_buffer->avg = (char *)calloc(1, size_of_element);
    ring_buffer->max_queue_length = buffer_length;
    ring_buffer->size_of_element = size_of_element;
    ring_buffer->current_queue_length = 0;
    ring_buffer->current_index = 0;
    return ring_buffer;
}

RingBuffer *ring_buffer_create(size_t size_of_element, size_t buffer_length)
{
    RingBuffer *ring_buffer = (RingBuffer *)calloc(sizeof(RingBuffer) + size_of_element * buffer_length, 1);
    if (!ring_buffer)
    {
        return (RingBuffer *)0;
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

void ring_buffer_destroy(RingBuffer *ring_buffer)
{
    free(ring_buffer);
}



/*
 * returns 1 if queue is full
 */
int ring_buffer_is_full(RingBuffer *ring_buffer)
{
    return ring_buffer->current_queue_length == ring_buffer->max_queue_length;
}

/*
 * returns 1 if queue is empty
 */
int ring_buffer_is_empty(RingBuffer *ring_buffer)
{
    return ring_buffer->current_queue_length == (size_t)0;
}

char *ring_buffer_get_element_pointer(RingBuffer *ring_buffer, size_t index)
{
    return ring_buffer->data_pointer + ring_buffer->size_of_element * index;
}

#define ring_buffer_get_element(ring_buffer, index, type) (*((type *)ring_buffer_get_element_pointer(ring_buffer, index)))

#define ring_buffer_get_element_at(ring_buffer, index, type) (*((type *)ring_buffer_get_element_pointer(ring_buffer, (index + ring_buffer->current_index) % ring_buffer->max_queue_length)))

float ring_buffer_get_element_at_float(RingBuffer *ring_buffer, int index)
{
    index += ring_buffer->current_index;
    index = index % ring_buffer->max_queue_length;
    if (index < 0)
    {
        index += ring_buffer->max_queue_length;
    }
    //assert(index >= 0);
    //assert(index < ring_buffer->max_queue_length);
    //index = index  % ring_buffer->max_queue_length > 0 ? index : ring_buffer->max_queue_length - index;
    return *((float *)ring_buffer_get_element_pointer(ring_buffer, index));
}

uint8_t ring_buffer_get_element_at_byte(RingBuffer *ring_buffer, int index)
{
    index += ring_buffer->current_index;
    index = index % ring_buffer->max_queue_length;
    if (index < 0)
    {
        index += ring_buffer->max_queue_length;
    }
    return *((uint8_t *)ring_buffer_get_element_pointer(ring_buffer, index));
}

/* for uint8_t only*/
uint8_t ring_buffer_get_max(RingBuffer *ring_buffer){
    uint8_t maxVal = 0;
    for(size_t i = 0; i < ring_buffer->current_queue_length; i++){
        uint8_t newVal = ring_buffer_get_element_at_byte(ring_buffer, i);
        maxVal = max(newVal, maxVal);
    }
    return maxVal;
}

/* for uint8_t only*/
uint8_t ring_buffer_get_min(RingBuffer *ring_buffer){
    uint8_t minVal = 0;
    for(size_t i = 0; i < ring_buffer->current_queue_length; i++){
        uint8_t newVal = ring_buffer_get_element_at_byte(ring_buffer, i);
        minVal = max(newVal, minVal);
    }
    return minVal;
}

uint8_t ring_buffer_get_last_element(RingBuffer *ring_buffer)
{
    if (ring_buffer_is_empty(ring_buffer))
    {
        return 0;
    }
    return ring_buffer_get_element_at_byte(ring_buffer, ring_buffer->current_queue_length - 1);
}

char *ring_buffer_get_last_element_pointer(RingBuffer *ring_buffer)
{
    if (ring_buffer_is_empty(ring_buffer))
    {
        return ring_buffer->data_pointer;
    }
    size_t nextFree = (ring_buffer->current_index + ring_buffer->current_queue_length - 1) % ring_buffer->max_queue_length;
    return ring_buffer_get_element_pointer(ring_buffer, nextFree);
}

void ring_buffer_inc_index(RingBuffer *queue)
{
    queue->current_index++;
    queue->current_index = queue->current_index % queue->max_queue_length;
}

int ring_buffer_pop(RingBuffer *ring_buffer, char *new_element)
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

int ring_buffer_push(RingBuffer *ring_buffer, const char *element)
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

int ring_buffer_push_overwrite(RingBuffer *ring_buffer, const char *element)
{
    if (!ring_buffer->current_queue_length)
    {
        memcpy(ring_buffer_get_element_pointer(ring_buffer, ring_buffer->current_index), element, ring_buffer->size_of_element);
    }
    else
    {
        size_t nextFree = (ring_buffer->current_index + ring_buffer->current_queue_length) % ring_buffer->max_queue_length;
        memcpy(ring_buffer_get_element_pointer(ring_buffer, nextFree), element, ring_buffer->size_of_element);
    }
    if (ring_buffer_is_full(ring_buffer))
    {
        ring_buffer_inc_index(ring_buffer);
    }
    else
    {
        ring_buffer->current_queue_length++;
    }

    return 0;
}

