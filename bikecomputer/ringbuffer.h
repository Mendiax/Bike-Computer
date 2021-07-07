#ifndef CUT_RINGBUFFER_H
#define CUT_RINGBUFFER_H

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

RingBuffer *ring_buffer_create(size_t size_of_element, size_t buffer_length)
{
    RingBuffer *ring_buffer = (RingBuffer *)calloc(sizeof(RingBuffer) + size_of_element * buffer_length, 1);
    if (!ring_buffer)
    {
        return (void *)0;
    }
    ring_buffer->min = (char*)calloc(1, size_of_element);
    ring_buffer->max = (char*)calloc(1, size_of_element);
    ring_buffer->avg = (char*)calloc(1, size_of_element);
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
static int ring_buffer_is_full(RingBuffer *ring_buffer)
{
    return ring_buffer->current_queue_length == ring_buffer->max_queue_length;
}

/*
 * returns 1 if queue is empty
 */
static int ring_buffer_is_empty(RingBuffer *ring_buffer)
{
    return ring_buffer->current_queue_length == (size_t)0;
}

static char *ring_buffer_get_element_pointer(RingBuffer *ring_buffer, size_t index)
{
    return ring_buffer->data_pointer + ring_buffer->size_of_element * index;
}

#define ring_buffer_get_element(ring_buffer, index, type) (*((type *)ring_buffer_get_element_pointer(ring_buffer, index)))

#define ring_buffer_get_element_at(ring_buffer, index, type) (*((type *)ring_buffer_get_element_pointer(ring_buffer, (index + ring_buffer->current_index) % ring_buffer->max_queue_length)))

static void ring_buffer_inc_index(RingBuffer *queue)
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

#endif //CUT_RINGBUFFER_H
