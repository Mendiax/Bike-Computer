#ifndef TYPES_H
#define TYPES_H

#include "ringbuffer.h"
#include "pico/sync.h"

#include <string>
#include <array>

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
    uint8_t sat;
    uint8_t sat2;
};

template<typename T, size_t N>
struct ArrayMinMaxS
{
    T array[N];
    T min;
    T max;
};

#define FORECAST_SENSOR_DATA_LEN 6

template<size_t N>
struct ForecastArrS
{
    ArrayMinMaxS<float, N> temperature_2m;
    ArrayMinMaxS<float, N> pressure_msl;
    ArrayMinMaxS<float, N> precipitation;
    ArrayMinMaxS<float, N> windspeed_10m;
    ArrayMinMaxS<float, N> winddirection_10m;
    ArrayMinMaxS<float, N> windgusts_10m;

    Time_HourS sunrise;
    Time_HourS sunset;
    float winddirection_10m_dominant;

    float current_windspeed;
    float current_winddirection;
};

struct Weather_BMP280_S
{
    float temperature;
    int32_t pressure;
};

// TODO optimize size
typedef struct SensorData
{
    ForecastArrS<FORECAST_SENSOR_DATA_LEN> forecast;
    Weather_BMP280_S weather;
    //RingBuffer* rearShockBuffer;
    SpeedData speed;
    GpsDataS gps_data;
    mtime_t time;
    Battery lipo;
    SystemState current_state;
    Time_HourS hour;
    Time_DateS date;
    float altitude;

    // char cipgsmloc[20];
    // char clbs[27];

} SensorData;

// variables used for mutlicore
extern mutex_t sensorDataMutex;
extern SensorData sensorData; // volatile ???

#endif
