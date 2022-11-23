#ifndef COMMON_DATA_HPP
#define COMMON_DATA_HPP

#include "session.hpp"
#include "common_types.h"
#include "ringbuffer.h"
#include <pico/sem.h>
#include "common_data.hpp"
#include "ringbuffer.h"

#define SEM_TIMEOUT_MS 1000

// variables used for mutlicore
// extern mutex_t sensorDataMutex;
// extern Session_Data* session_p;

template<typename T>
class Data_Queue
{
private:
    mutex_t pc_mutex;
    semaphore_t number_of_queueing_portions;
    semaphore_t number_of_empty_positions;
    Ring_Buffer* pc_queue;
public:

    void push_blocking(const T& packet);
    void pop_blocking(T& packet);

    Data_Queue(uint max_len);
};

template<typename T>
Data_Queue<T>::Data_Queue(uint max_len)
{
    pc_queue = ring_buffer_create(sizeof(T), max_len);
    mutex_init(&pc_mutex);
    sem_init(&number_of_queueing_portions, 0, max_len);
    sem_init(&number_of_empty_positions, max_len, max_len);
}

template<typename T>
void Data_Queue<T>::pop_blocking(T& packet)
{
    if(sem_acquire_timeout_ms(&number_of_queueing_portions, SEM_TIMEOUT_MS))
    {
        ring_buffer_pop(pc_queue, (char*) &packet);
        sem_release(&number_of_empty_positions);
    }
}
template<typename T>
void Data_Queue<T>::push_blocking(const T& packet)
{
    if(sem_acquire_timeout_ms(&number_of_empty_positions, SEM_TIMEOUT_MS))
    {

        if (!ring_buffer_is_full(pc_queue)) {
            ring_buffer_push(pc_queue, (char*) &packet);
        }
        sem_release(&number_of_queueing_portions);
    }
}

#endif
