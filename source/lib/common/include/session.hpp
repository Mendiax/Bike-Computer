#ifndef SESSION_HPP
#define SESSION_HPP

#include "common_types.h"
#include "csv_interface.hpp"
#include <string.h>

#define SESION_DATA_CSV_LEN 100

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
        memset(&this->speed, 0, sizeof(this->speed));
        memset(&this->time_start, 0, sizeof(this->time_start));
        memset(&this->time_end, 0, sizeof(this->time_end));

        this->status = Status::NOT_STARTED;
        // this->speed = {0};
        // this->time_start = {0};
        // this->time_end = {0};
    }
public:
    Session_Data();
    Session_Data(const char* csv_line);

    inline Status get_status() const {return this->status;}

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

    void start(TimeS time);
    void pause();
    void cont();
    void end(TimeS time);
    void update(float speed_kph, float distance_m);


    // csv interface
    const char *get_header();
    std::string get_line();
};

#endif