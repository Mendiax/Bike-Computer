#include "common_data.hpp"
#include "ringbuffer.h"

Session_Data *session_p = 0;
Sensor_Data sensors_data = {0};
mutex_t sensorDataMutex;



semaphore_t number_of_queueing_portions;
semaphore_t number_of_empty_positions;

mutex_t pc_mutex;

Ring_Buffer* common_memory::get_pc_queue()
{
    static Ring_Buffer* pc_queue = 0;
    if(pc_queue == 0)
    {
        pc_queue = ring_buffer_create(sizeof(common_memory::Packet), QUEUE_LEN);
    }
    return pc_queue;
}

