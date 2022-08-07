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
    uint64_t driveTime; // in ms
};

// typedef struct mtime_t
// {
//     unsigned int t;
//     char xddd[2];
// } mtime_t;

typedef struct SensorData
{
    RingBuffer* rearShockBuffer;
    SpeedData speed;
    uint64_t time;
    bool lipoCharging;
    int lipoLevel; // in %
} SensorData;

#endif
