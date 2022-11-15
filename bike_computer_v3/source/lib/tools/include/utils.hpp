#ifndef COMMON_UTILS_HPP
#define COMMON_UTILS_HPP

#include "common_types.h"
#include "parser.hpp"

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

class Unique_Mutex
{
    mutex_t* mutex_p;
public:
    Unique_Mutex(mutex_t* mutex_p)
        :mutex_p{mutex_p}
    {
        mutex_enter_blocking(mutex_p);
    }
    ~Unique_Mutex()
    {
        mutex_exit(mutex_p);
    }
};

/**
 * @brief checks avaible memory with malloc
 *
 */
size_t check_free_mem();

TimeS time_from_str(const char* str);
std::string time_to_str_file_name_conv(const TimeS& time);
std::string time_to_str(const TimeS& time);
std::string time_to_str(const Time_HourS& time);

Time_HourS time_from_millis(uint64_t millis_to_add);


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



#endif