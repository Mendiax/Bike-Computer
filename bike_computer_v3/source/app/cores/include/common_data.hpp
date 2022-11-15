#ifndef COMMON_DATA_HPP
#define COMMON_DATA_HPP

#include "session.hpp"
#include "common_types.h"

// variables used for mutlicore
extern mutex_t sensorDataMutex;
extern Session_Data* session_p;
extern Sensor_Data sensors_data;

#endif
