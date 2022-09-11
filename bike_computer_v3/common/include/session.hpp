#ifndef SESSION_HPP
#define SESSION_HPP

#include "common_types.h"
#include "csv_interface.hpp"

/**
 * @brief Contains data about current session
 *
 */
class Session_Data : public Csv_Interface
{
public:
    TimeS time_start;
    TimeS time_end;
    SpeedData speed;
    Gear_Usage gear_usage;

    enum class Status {
        STOPPED, RUNNING, ENDED
    };
private:
    Status status;
    // time data
    absolute_time_t absolute_time_start;
    absolute_time_t absolute_time_last_stop;

public:
    Session_Data();

    void start(TimeS time);
    void stop();
    void cont();
    void end(TimeS time);
    // update data
    /**
     * @brief add millis to correct gear
     *
     * @param gear idx of gear
     * @param cadence
     * @param millis
     */
    void add_gear_time(uint8_t gear, float cadence, uint64_t millis);
    int16_t* get_distance_var();
    void update(float speed_kph, float distance_m);

    // csv interface
    const char *get_header();
    std::string get_line();
};

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
    Session(Session_Data session_data); // TODO
    void start(TimeS time_start);
    void end(const TimeS& time_end, const  SpeedData& data);
    std::string to_string();

    // csv interface
    const char* get_header();
    std::string get_line();
};

#endif