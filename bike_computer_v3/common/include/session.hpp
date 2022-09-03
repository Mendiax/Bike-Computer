#ifndef SESSION_HPP
#define SESSION_HPP

#include "common_types.h"
#include "csv_interface.hpp"

/**
 * @brief structure that will be saved in memory
 * 
 */
class Session : public Csv_Interface
{
private:
    TimeS time_start, time_end; // absolute values
    Time_HourS duration; //only pedaling
    float velocity_max;
    float velocity_avg;
    float velocity_avg_global; // with absolute time
    int32_t distance; // in m
    Gear_Usage gears; // TODO
    // TODO weather ???
public:
    Session();
    void start(TimeS time_start);
    void end(const TimeS& time_end, const  SpeedData& data);
    std::string to_string();

    // csv interface
    const char* get_header();
    std::string get_line();
};

#endif