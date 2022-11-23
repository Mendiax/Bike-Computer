#ifndef COMMON_UTILS_HPP
#define COMMON_UTILS_HPP

#include "common_types.h"
#include "parser.hpp"
#include "pico/types.h"

#include <pico/mutex.h>

#include <string>
#include <inttypes.h>
#include <stdlib.h>
#include <vector>


/**
 * @brief file containg functions for common types
 *
 */

/**
 * @brief splits given string by given sepparator
 *
 * @param string
 * @param sep
 * @return std::vector<std::string>
 */
std::vector<std::string> split_string(const std::string& string, char sep = ',');

static inline bool is_leap_year(int year)
{
  return (year % 4 == 0) && ( (year % 100 != 0) || (year % 400 == 0) );
}

static inline bool is_last_day_in_month(int year, int month, int day)
{
  return ((month == 1 && day == 31) ||
          (is_leap_year(year) ? (month == 2 && day == 29) : (month == 2 && day == 28)) ||
          (month == 3 && day == 31) ||
          (month == 4 && day == 30) ||
          (month == 5 && day == 31) ||
          (month == 6 && day == 30) ||
          (month == 7 && day == 31) ||
          (month == 8 && day == 31) ||
          (month == 9 && day == 30) ||
          (month == 10 && day == 31) ||
          (month == 11 && day == 30) ||
          (month == 12 && day == 31)
 );
}

template <typename T>
void change_time_by_hour(T* t, int offset)
{
    offset = offset % 24;
    if((int)t->hour + offset < 0)
    {
        t->hour = ((int)t->hour + 24) - offset;

         // decrease by one day
        if (t->day == 1)
        {
            if(t->month == 1)
            {
                t->year--;
                t->month = 12;
            }
            while (!is_last_day_in_month(t->year, t->month, t->day))
            {
                t->day--;
            }
        }
        else
        {
            t->day--;
        }

    } else if((int)t->hour + offset > 23)
    {
        t->hour = t->hour + offset - 24;
        // increase by one day
        if (is_last_day_in_month(t->year, t->month, t->day))
        {
            if (t->month == 12)
            {
                t->month = 1;
                t->day = 1;
                t->year++;
            }
            else
            {
                t->day = 1;
                t->month++;
            }
        }
        else
        {
            t->day++;
        }
    } else
    {
        t->hour = offset + t->hour;
    }

}

/**
 * @brief checks avaible memory with malloc
 *
 */
size_t check_free_mem();

TimeS time_from_str(const char* str);
std::string time_to_str_file_name_conv(const TimeS& time);
std::string time_to_str(const TimeS& time);
std::string time_to_str(const Time_HourS& time);

void time_print(const TimeS& time);


static inline absolute_time_t absolute_time_copy_volatile(volatile absolute_time_t* time)
{
    #ifdef NDEBUG
    return *time;
    #else
    return {time->_private_us_since_boot};
    #endif
}
static inline void absolute_time_copy_to_volatile(volatile absolute_time_t& time, absolute_time_t timeToCopy)
{
    #ifdef NDEBUG
    time = timeToCopy;
    #else
    time._private_us_since_boot = timeToCopy._private_us_since_boot;
    #endif
}

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
        array.min = std::min(array.min, array.array[i]);
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



#endif