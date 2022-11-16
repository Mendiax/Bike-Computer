#ifndef COMMON_DATA_HPP
#define COMMON_DATA_HPP

#include "session.hpp"
#include "common_types.h"
#include "ringbuffer.h"

// variables used for mutlicore
extern mutex_t sensorDataMutex;
extern Session_Data* session_p;
extern Sensor_Data sensors_data;


namespace common_memory
{
    struct Packet{
        Sensor_Data sensors;
        Session_Data session;
    };
    Ring_Buffer pc_queue;
};

#endif
