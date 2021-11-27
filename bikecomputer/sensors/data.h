#ifndef __SENSORDATA_H__
#define __SENSORDATA_H__

#include "../addons/ringbuffer.h"


typedef struct SensorData
{
    RingBuffer* rearShockBuffer;
    RingBuffer* speedBuffer;
    float speedMax;
    float speedAvgVal;
    unsigned long speedAvgCnt;
    unsigned speedDistance;
    unsigned long time;
} SensorData;


#endif