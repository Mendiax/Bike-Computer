#ifndef TYPES_H
#define TYPES_H

#include "ringbuffer.h"

struct SpeedData
{
    RingBuffer* speedBuffer;
    float speedMax;
    float speedAvgVal;
    unsigned long speedAvgCnt;
    unsigned speedDistance;
    unsigned speedDistanceHundreth;
};


typedef struct SensorData
{
    RingBuffer* rearShockBuffer;
    SpeedData speed;
    unsigned long time;
    bool lipoCharging;
    int lipoLevel; // in %
} SensorData;

#endif
