#ifndef SESSION_HPP
#define SESSION_HPP

#include "common_types.h"
#include "csv_interface.hpp"

#define SESION_DATA_CSV_LEN_NO_GEARS 100

/**
 * @brief Contains data about current session
 *
 */
class Session_Data : public Csv_Interface
{
public:
    uint16_t id;
    TimeS time_start;
    TimeS time_end;
    SpeedData speed;
    Gear_Usage gear_usage;

    enum class Status {
        NOT_STARTED, PAUSED, RUNNING, ENDED
    };
private:
    Status status;
    // time data
    absolute_time_t absolute_time_start;
    absolute_time_t absolute_time_last_stop;

    inline void reset()
    {
        this->speed = {0};
        this->gear_usage = {0};
        this->status = Status::NOT_STARTED;
        this->time_start = {0};
        this->time_end = {0};
    }
public:
    Session_Data();
    /**
     * @brief Construct a new Session_Data from csv line
     *
     * @param csv_line
     */
    Session_Data(const char* csv_line, bool load_gear=true);

    inline void set_id(uint16_t id){this->id = id;}
    inline bool is_running(){
        switch (this->status)
        {
            case Session_Data::Status::RUNNING:
                return true;
            case Session_Data::Status::PAUSED:
            case Session_Data::Status::NOT_STARTED:
            case Session_Data::Status::ENDED:
            default:
                return false;
        }
    }
    inline bool has_started(){
        switch (this->status)
        {
            case Session_Data::Status::RUNNING:
            case Session_Data::Status::PAUSED:
                return true;
            case Session_Data::Status::NOT_STARTED:
            case Session_Data::Status::ENDED:
            default:
                return false;
        }
    }

    inline TimeS get_start_time() const {return this->time_start;}
    inline void set_start_time(TimeS time) {if(has_started() && !time_start.is_valid()) this->time_start = time;}

    inline absolute_time_t get_start_absolute_time() {return this->absolute_time_start;}


    void start(TimeS time);
    void pause();
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

#endif