#ifndef TYPES_H
#define TYPES_H

#include "ringbuffer.h"
#include "pico/sync.h"

enum class SystemState
{
    RUNNING,
    PAUSED,
    STOPPED,
    CHARGING,
    TURNED_ON
};

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

struct TimeS
{
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minutes;
    float seconds;
};

struct Time_DateS
{
    uint16_t year;
    uint8_t month;
    uint8_t day;
};

struct Time_HourS
{
    uint8_t hour;
    uint8_t minutes;
    float seconds;
};


// for special prints we need seperate types in structs
struct mtime_t
{
    uint64_t t;
};

struct Battery
{
    bool is_charging;
    int8_t level; // in %
};
// typedef struct mtime_t
// {
//     unsigned int t;
//     char xddd[2];
// } mtime_t;
struct GpsDataS
{
    float speed;
    float lat;
    float lon;
    float msl;
};

typedef struct SensorData
{
    RingBuffer* rearShockBuffer;
    SpeedData speed;
    GpsDataS gps_data;
    mtime_t time;
    Battery lipo;
    SystemState current_state;
    Time_HourS hour;
    Time_DateS date;
} SensorData;

// variables used for mutlicore
extern mutex_t sensorDataMutex;
extern SensorData sensorData; // volatile ???

#endif
