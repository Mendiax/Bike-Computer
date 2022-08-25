#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <inttypes.h>
#include <vector>
#include <cstring>
#include <assert.h>
#include <iostream>
#include <unordered_map>


#define DATA_PER_DAY 24



struct TimeIso8601S
{
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minutes;
};

#define _PRIu16 "hu"

#define _PRIu8 "hhu"

static inline void TimeIso8601_print(const TimeIso8601S& time)
{
    std::cout << (int)time.year << " " << (int)time.month << " " << (int)time.day << " " << (int)time.hour << " " << (int)time.minutes << std::endl;
}


static inline TimeIso8601S* TimeIso8601_from_string(const std::string& time_str)
{
    // std::cout << time_str << std::endl;
    // std::cout << time_str.at(11) << std::endl;

    TimeIso8601S* new_time = new TimeIso8601S();
    memset(new_time, 0, sizeof(TimeIso8601S));
    if(time_str.at(11) == 'T')
    {   //_1____6__9__12_15 
        //"2022-08-19T19:54"
        new_time->year = std::atoi(time_str.substr(1,4).c_str());
        new_time->month = std::atoi(time_str.substr(6,2).c_str());
        new_time->day = std::atoi(time_str.substr(9,2).c_str());
        new_time->hour = std::atoi(time_str.substr(12,2).c_str());
        new_time->minutes = std::atoi(time_str.substr(15,2).c_str());
    }
    else
    {
        new_time->year = std::atoi(time_str.substr(1,4).c_str());
        new_time->month = std::atoi(time_str.substr(6,2).c_str());
        new_time->day = std::atoi(time_str.substr(9,2).c_str());
    }
    assert(new_time->year != 0);
    return new_time;
}

struct WeatherS
{
    float temperature;
    float windspeed;
    float winddirection;
};


// static const char* unit_hourly_time = "iso8601";
// static const char* unit_hourly_temperature_2m = "°C";
// static const char* unit_hourly_pressure_msl = "hPa";
// static const char* unit_hourly_precipitation = "mm";
// static const char* unit_hourly_windspeed_10m = "km/h";
// static const char* unit_hourly_winddirection_10m = "°";
// static const char* unit_hourly_windgusts_10m = "km/h";
// static const char* unit_daily_time = "iso8601";
// static const char* unit_daily_sunrise = "iso8601";
// static const char* unit_daily_sunset = "iso8601";
// static const char* unit_daily_winddirection_10m_dominant = "°";

struct ForecastS
{
    uint8_t data_len;
    float latitude;
    float longitude;
    WeatherS current_weather;
    float* temperature_2m;
    float* pressure_msl;
    float* precipitation;
    float* windspeed_10m;
    float* winddirection_10m;
    float* windgusts_10m;
    // daily
    TimeIso8601S* dates;
    TimeIso8601S* sunrise;
    TimeIso8601S* sunset;
    float* winddirection_10m_dominant;

    ForecastS(uint8_t days);
    ~ForecastS();
};


static inline void create_offsets(std::unordered_map<std::string, void*>& map, const ForecastS& structure)
{
    map["\"latitude\""] = (void*)&structure.latitude;
    map["\"longitude\""] = (void*)&structure.longitude;
    //current
    map["\"temperature\""] = (void*)&structure.current_weather.temperature;
    map["\"windspeed\""] = (void*)&structure.current_weather.windspeed;
    map["\"winddirection\""] = (void*)&structure.current_weather.winddirection;
    // hourly
    map["\"temperature_2m\""] = (void*)&structure.temperature_2m;
    map["\"pressure_msl\""] = (void*)&structure.pressure_msl;
    map["\"precipitation\""] = (void*)&structure.precipitation;
    map["\"windspeed_10m\""] = (void*)&structure.windspeed_10m;
    map["\"winddirection_10m\""] = (void*)&structure.winddirection_10m;
    map["\"windgusts_10m\""] = (void*)&structure.windgusts_10m;
    // daily
    map["\"time\""] = (void*)&structure.dates;
    map["\"sunrise\""] = (void*)&structure.sunrise;
    map["\"sunset\""] = (void*)&structure.sunset;
    map["\"winddirection_10m_dominant\""] = (void*)&structure.winddirection_10m_dominant;
}



[[nodiscard]]
ForecastS* parse_json(const std::string& json, uint8_t days);




#endif