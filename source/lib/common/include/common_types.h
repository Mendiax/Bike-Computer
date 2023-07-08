#ifndef TYPES_H
#define TYPES_H

#include "bike_config.hpp"
// #include "display/driver.hpp"
#include "gear_suggestion.hpp"

#include "pico/sync.h"
#include "pico/util/datetime.h"

#include <string>
#include <array>
#include <vector>

struct SpeedData
{
    uint64_t drive_time; // in s
    uint64_t stop_time; // in ms
    int16_t distance;
    float velocity;
    float velocityMax;
    float avg;
    int8_t distanceDec;
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
    void update_time(absolute_time_t current); // not used
    void substract_ms(uint32_t ms);
    void from_date_time(const datetime_t& date);
    datetime_t to_date_time() const;

    bool is_valid() const;
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

struct Weather_BMP280_S
{
    float pressure;
    float temperature;
};

struct RGB
{
    int8_t r, g, b;
};

struct Gear_Suggestions
{
    float cadence_min;
    float cadence_max;
    RGB gear_suggestion_color;
    char gear_suggestion[GEAR_SUGGESTION_LEN + 1];
};

/**
 * @brief Contains data from sensors
 *
 */
typedef struct Sensor_Data
{
    TimeS current_time;
    Weather_BMP280_S weather;
    GpsDataS gps_data;
    Battery lipo; // battery info
    Gear_S gear;  // gear {front, rear}
    // SystemState current_state;
    // gear suggestions
    Gear_Suggestions gear_suggestions;
    float altitude; // height in m
    float cadence;  // rpm
    float velocity; // speed in kph
    float accel; // in km/h/s
    float slope; // slope in %
    float total_time_ridden; // in h
    float total_distance_ridden; // in km

    float velocity_raw;

} Sensor_Data;

#endif
