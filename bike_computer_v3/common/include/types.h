#ifndef TYPES_H
#define TYPES_H

#include "ringbuffer.h"

struct SpeedData
{
    //RingBuffer* speedBuffer;
    float velocity;
    float velocityMax;
    float avgGlobal;
    int16_t distance;
    int8_t distanceDec;
    uint64_t driveTime;
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
