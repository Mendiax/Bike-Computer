
// #------------------------------#
// |          includes            |
// #------------------------------#
// pico includes

// c/c++ includes

// my includes
#include "hardware/rtc.h"
#include "pico/types.h"
#include "timers.hpp"
#include "traces.h"

#include <cstddef>
#include <cstring>
#include <ctime>
#include <thread>

// #------------------------------#
// |           macros             |
// #------------------------------#

// #------------------------------#
// | global variables definitions |
// #------------------------------#

// #------------------------------#
// | static variables definitions |
// #------------------------------#
static datetime_t rtc_timer;
// #------------------------------#
// | static functions declarations|
// #------------------------------#
static bool clock_kernel(repeating_timer_t* timer_p);
// #------------------------------#
// | global function definitions  |
// #------------------------------#
void rtc_init()
{
    static repeating_timer_t timer;
    add_repeating_timer_ms(1000, clock_kernel, NULL, &timer);
}
void rtc_set_datetime(datetime_t* time_p)
{
    memcpy(&rtc_timer, time_p, sizeof(datetime_t));
}
void rtc_get_datetime(datetime_t* time_p)
{
    memcpy(time_p, &rtc_timer, sizeof(datetime_t));
}
// #------------------------------#
// | static functions definitions |
// #------------------------------#

static bool clock_kernel(repeating_timer_t* time_p)
{
    // add 1 sec to timer TODO
    const auto currentTime = std::chrono::system_clock::now();
    const  std::time_t currentTimeT = std::chrono::system_clock::to_time_t(currentTime);
    std::tm timeInfo;
    localtime_r(&currentTimeT, &timeInfo);

    datetime_t hour = {
        (int16_t)timeInfo.tm_year,
        (int8_t)timeInfo.tm_mon,
        (int8_t)timeInfo.tm_mday,
        (int8_t)timeInfo.tm_wday,
        (int8_t)timeInfo.tm_hour,
        (int8_t)timeInfo.tm_min,
        (int8_t)timeInfo.tm_sec
    };
    rtc_timer = hour;

    TRACE_DEBUG(0, TRACE_HOST, "clock added second\n");

    return true;
}