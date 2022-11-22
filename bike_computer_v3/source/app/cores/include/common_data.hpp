#ifndef COMMON_DATA_HPP
#define COMMON_DATA_HPP

#include "session.hpp"
#include "common_types.h"
#include "ringbuffer.h"
#include <pico/sem.h>

#define QUEUE_LEN 2

// variables used for mutlicore
extern mutex_t sensorDataMutex;
extern Session_Data* session_p;
extern Sensor_Data sensors_data;

extern mutex_t pc_mutex;

extern semaphore_t number_of_queueing_portions;
extern semaphore_t number_of_empty_positions;

namespace common_memory
{
    struct Packet{
        Sensor_Data sensors;
        Session_Data session;
    };
    Ring_Buffer* get_pc_queue();
};

#endif
