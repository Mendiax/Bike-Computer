#include "common_data.hpp"
#include "ringbuffer.h"


// mutex_t sensorDataMutex;



semaphore_t common_data::number_of_queueing_portions;
semaphore_t common_data::number_of_empty_positions;

mutex_t common_data::pc_mutex;

Ring_Buffer* common_data::get_pc_queue()
{
    static Ring_Buffer* pc_queue = 0;
    if(pc_queue == 0)
    {
        pc_queue = ring_buffer_create(sizeof(common_data::Packet), QUEUE_LEN);
    }
    return pc_queue;
}

