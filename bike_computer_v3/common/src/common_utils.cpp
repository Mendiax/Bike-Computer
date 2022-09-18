#include "common_utils.hpp"
#include "traces.h"
#include "math.h"


std::vector<std::string> split_string(const std::string& string, char sep)
{
    std::vector<std::string> strings; // this will be returned

    size_t begin_idx, end_idx;
    begin_idx = 0;
    while((end_idx = string.find(sep, begin_idx)) != std::string::npos)
    {
        //end_idx = string.find(sep, begin_idx);
        strings.emplace_back(string.substr(begin_idx, end_idx - begin_idx));
        begin_idx = end_idx + 1;
    }
    if(begin_idx < string.length())
        strings.emplace_back(string.substr(begin_idx));
    return strings;
}

size_t check_free_mem()
{
    size_t avaible_memory = SIZE_MAX;
    void* mem;
    while((avaible_memory != 0) && (mem = malloc(avaible_memory)) == NULL)
    {
        avaible_memory >>= 1;
    }
    if(mem != NULL)
        free(mem);
    return avaible_memory;
}

std::string time_to_str(const Time_HourS& time)
{
    enum {BUFFER_SIZE=13};
    char buffer[BUFFER_SIZE] = {0};

    snprintf(buffer, BUFFER_SIZE,
            "%02" PRIu8 ":"
            "%02" PRIu8 ":"
            "%02.2f",
            time.hour,
            time.minutes,
            time.seconds);
    return std::string(buffer);
}


std::string time_to_str(const TimeS& time)
{
    enum {BUFFER_SIZE=24};
    char buffer[BUFFER_SIZE] = {0};

    snprintf(buffer, BUFFER_SIZE,
            "%04" PRIu16 "."
            "%02" PRIu8 "."
            "%02" PRIu8 ","
            "%02" PRIu8 ":"
            "%02" PRIu8 ":"
            "%05.2f",
            time.year,
            time.month,
            time.day,
            time.hour,
            time.minutes,
            time.seconds);
    return std::string(buffer);
}

void time_print(const TimeS& time)
{
    PRINTF("time: %s \n", time_to_str(time).c_str());
}


Time_HourS time_from_millis(uint64_t millis_to_add)
{
    Time_HourS correct_time;
    correct_time.seconds = ((float)millis_to_add / 1000.0f);
    uint64_t minutes_to_add = std::floor(correct_time.seconds / 60.0f);
    correct_time.seconds -= (float)minutes_to_add * 60.0f;

    uint64_t hours_to_add = minutes_to_add / 60;
    minutes_to_add = minutes_to_add % 60;
    correct_time.minutes = minutes_to_add;

    uint64_t days_to_add = hours_to_add / 24;
    hours_to_add = hours_to_add % 24;
    correct_time.hour = hours_to_add;

    return correct_time;
}