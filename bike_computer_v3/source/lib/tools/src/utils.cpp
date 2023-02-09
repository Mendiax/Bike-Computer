#include "utils.hpp"
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
    size_t avaible_memory = 0;
    size_t chunk_size = 262144; // 2^18
    void* mem_p[12] = {0}; // holds allocated memory
    int i;
    for(i = 0; i < 10; i++)
    {
        mem_p[i] = malloc(chunk_size);
        if(mem_p[i] != NULL){avaible_memory += chunk_size;}
        chunk_size >>= 1;
    }
    for(i = 0; i < 12; i++)
    {
        if(mem_p[i] != NULL){free(mem_p[i]);}
    }
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
TimeS time_from_str(const char* str)
{
    TimeS time;
    // "yyyy.MM.dd,hh:mm:ss"
    auto date_hour = split_string(str, ',');
    if(date_hour.size() != 2)
    {
        // error
        return time;
    }
    auto date = split_string(date_hour[0], '.');
    if(date.size() != 3)
    {
        // error
        return time;
    }
    time.year = atoi(date[0].c_str());
    time.month = atoi(date[1].c_str());
    time.day = atoi(date[2].c_str());

    auto hour = split_string(date_hour[1], ':');
    if(hour.size() != 3)
    {
        // error
        return time;
    }
    time.hour = atoi(hour[0].c_str());
    time.minutes = atoi(hour[1].c_str());
    time.seconds = atoi(hour[2].c_str());

    return time;
}

std::string time_to_str_file_name_conv(const TimeS& time)
{
    enum {BUFFER_SIZE=24};
    char buffer[BUFFER_SIZE] = {0};

    snprintf(buffer, BUFFER_SIZE,
            "%04" PRIu16 "_"
            "%02" PRIu8 "_"
            "%02" PRIu8 "_"
            "%02" PRIu8 "_"
            "%02" PRIu8,
            time.year,
            time.month,
            time.day,
            time.hour,
            time.minutes);
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
