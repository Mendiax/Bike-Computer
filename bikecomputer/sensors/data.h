#ifndef __SENSORDATA_H__
#define __SENSORDATA_H__

#include "../addons/ringbuffer.h"

typedef struct SensorData
{
    RingBuffer* speedBuffer;
    float speedMax;
    float speedAvgVal;
    unsigned long speedAvgCnt;
} SensorData;


#endif