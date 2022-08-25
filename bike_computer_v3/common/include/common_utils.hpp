#ifndef COMMON_UTILS_HPP
#define COMMON_UTILS_HPP

#include "common_types.h"
#include "parser.hpp"

#include <string>
#include <inttypes.h>


/**
 * @brief file containg functions for common types
 * 
 */

/**
 * @brief Get the iso format of given structure
 * 
 * @tparam T 
 * @param time 
 * @return std::string 
 */
template <typename T>
static inline std::string get_iso_format(const T& time)
{
    // 2022-08-19
    char iso_format[11] = {0};
    sprintf(iso_format, "%4" PRIu16 "-%02" PRIu8 "-%02" PRIu8, time.year, time.month, time.day);
    return iso_format;
}

template<typename T, size_t N>
static inline void set_minmax(ArrayMinMaxS<T,N>& array)
{
    for(size_t i = 0; i< N; ++i)
    {
        array.min = std::max(array.min, array.array[i]);
        array.max = std::max(array.max, array.array[i]);
    }
}

template<typename T, size_t N>
static inline void print(ArrayMinMaxS<T,N>& array)
{
    for(size_t i = 0; i< N; ++i)
    {
       std::cout << array.array[i] << " ";
    }
    std::cout << std::endl;
}

template<size_t N>
static inline void move_forecasts_to_forecastplots(ForecastS* raw_data, ForecastArrS<N>* forecast, uint8_t current_hour=0)
{
    if(current_hour + N > raw_data->data_len)
    {
        current_hour = raw_data->data_len - N;
    }
    memcpy(&forecast->temperature_2m.array[0], &raw_data->temperature_2m[current_hour], sizeof(*raw_data->temperature_2m) * FORECAST_SENSOR_DATA_LEN);
    memcpy(&forecast->pressure_msl.array[0], &raw_data->pressure_msl[current_hour], sizeof(*raw_data->pressure_msl) * FORECAST_SENSOR_DATA_LEN);
    memcpy(&forecast->precipitation.array[0], &raw_data->precipitation[current_hour], sizeof(*raw_data->precipitation) * FORECAST_SENSOR_DATA_LEN);
    memcpy(&forecast->windspeed_10m.array[0], &raw_data->windspeed_10m[current_hour], sizeof(*raw_data->windspeed_10m) * FORECAST_SENSOR_DATA_LEN);
    memcpy(&forecast->winddirection_10m.array[0], &raw_data->winddirection_10m[current_hour], sizeof(*raw_data->winddirection_10m) * FORECAST_SENSOR_DATA_LEN);
    memcpy(&forecast->windgusts_10m.array[0], &raw_data->windgusts_10m[current_hour], sizeof(*raw_data->windgusts_10m) * FORECAST_SENSOR_DATA_LEN);

    forecast->sunrise.hour = raw_data->sunrise[0].hour;
    forecast->sunrise.minutes = raw_data->sunrise[0].minutes;
    forecast->sunrise.seconds = 0;
    
    forecast->sunset.hour = raw_data->sunset[0].hour;
    forecast->sunset.minutes = raw_data->sunset[0].minutes;
    forecast->sunset.seconds = 0;

    forecast->winddirection_10m_dominant = raw_data->winddirection_10m_dominant[0];
    forecast->current_windspeed = raw_data->current_weather.windspeed;
    forecast->current_winddirection = raw_data->current_weather.winddirection;

    delete raw_data;
}

#endif