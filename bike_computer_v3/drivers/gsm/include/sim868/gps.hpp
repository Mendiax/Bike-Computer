#ifndef SIM868_GPS_HPP
#define SIM868_GPS_HPP

#include <stdint.h>
#include <string>
#include "pico/time.h"
#include "common_types.h"

enum class GpsState
{
    OFF, // turn off
    NO_RESPOND, // no respond from sim868
    NO_SIGNAL, // no connection to satelites (or not enough)
    DATA_AVAIBLE, // data can be read
    POSITION_AVAIBLE,
    RESTARTING
};

struct GpsRawData
{
    bool status; // id 1
    TimeS utc_date_time; // 3
    float latitude; // <id 4>
    float longitude; // <id 5>
    float msl_m; // <id 6> in meteres above the sea
    float speed_kph; // <id 7>
    uint8_t gps_satelites; // <id 15>
    uint8_t gnss_satelites; // <id 16>
    uint8_t glonass_satelites; // <id 17>
    absolute_time_t data_time_stamp;
};

void time_print(const TimeS& time);

void get_time_from_str(TimeS& time, const std::string& str);

/**
 * @brief corrects time based on time passed from last fetch with correct time
 * 
 * @param time 
 * @return true if date was fetch from module
 * @return false 
 */
TimeS correct_time(const GpsRawData& time);





namespace sim868
{
    namespace gps
    {
        void turn_on();
        void turn_off();

        GpsRawData get_gps_from_respond(const std::string& respond);
        bool fetch_data();
        // void check_satelites();
        // void check_date();

        // getters
        bool get_speed(float& speed);
        bool get_position(float& latitude, float& longitude);
        bool get_date(TimeS& time);
        bool get_msl(float& msl);
        GpsState get_gps_state(void);
    };
};


#endif
