#include "common_data.hpp"

Session_Data *session_p = 0;
Sensor_Data sensors_data = {0};
mutex_t sensorDataMutex;
