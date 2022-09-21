#ifndef TYPES_H
#define TYPES_H

#include "ringbuffer.h"
#include "bike_config.hpp"

#include "pico/sync.h"

#include <string>
#include <array>
#include <vector>

enum class SystemState
{
    TURNED_ON,
    // SESSION_AUTOSTART,
    AUTOSTART,
    RUNNING,
    PAUSED,
    ENDED,
    CHARGING
};

struct SpeedData
{
    //RingBuffer* speedBuffer;
    float velocity;
    float velocityMax;
    float avg;
    float avg_global;
    int16_t distance;
    int8_t distanceDec;
    uint64_t drive_time; // in s
    uint64_t stop_time; // in ms
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

struct TimeS
{
    union
    {
        struct
        {
            Time_DateS date;
            Time_HourS hours;
        };
        struct
        {
            uint16_t year;
            uint8_t month;
            uint8_t day;
            uint8_t hour;
            uint8_t minutes;
            float seconds;
        };
    };
    absolute_time_t time_stamp;
    void update_time(absolute_time_t current);
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

#define FORECAST_SENSOR_DATA_LEN 24

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



// void reset_session_data(Session_Data& data)
// {
//     memset(&data, 0 ,sizeof(data));
// }


/**
 * @brief Contains data from sensors
 *
 */
typedef struct Sensor_Data // TODO optimize size
{
    ForecastArrS<FORECAST_SENSOR_DATA_LEN> forecast;
    TimeS current_time;
    Weather_BMP280_S weather;
    float altitude; // height in m
    float cadence;  // rpm
    float velocity; // speed in kph
    float slope; // slope in %
    float total_time_ridden; // in h
    float total_distance_ridden; // in km
    GpsDataS gps_data;
    Battery lipo; // battery info
    Gear_S gear;  // gear {front, rear}
    SystemState current_state;

    // char cipgsmloc[20];
    // char clbs[27];

} Sensor_Data;

#endif
