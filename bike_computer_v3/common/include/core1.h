#ifndef CORE1_H
#define CORE1_H

#include <pico/sync.h>
#include "types.h"

// variables used for mutlicore
extern mutex_t sensorDataMutex;
extern SensorData sensorData; // volatile ???

void core1LaunchThread(void);

#endif