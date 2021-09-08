#ifndef __SENSORDATA_H__
#define __SENSORDATA_H__

#include "../addons/ringbuffer.h"

typedef struct SensorData
{
    RingBuffer* speedBuffer;
    double speedMax;
    double speedAvgVal;
    unsigned long speedAvgCnt;
} SensorData;


#endif