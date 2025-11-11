#ifndef SIM868_GPS_HPP
#define SIM868_GPS_HPP

// #-------------------------------#
// |           includes            |
// #-------------------------------#

// c/c++ includes
#include <stdint.h>
#include <string>

// my includes
#include "pico/time.h"
#include "common_types.h"

// #-------------------------------#
// |            macros             |
// #-------------------------------#

// #-------------------------------#
// | global types declarations     |
// #-------------------------------#

/**
 * @brief Enum class representing the state of the GPS module.
 */
enum class GpsState
{
    OFF,             ///< GPS is turned off.
    NO_RESPOND,      ///< No response from the SIM868 module.
    NO_SIGNAL,       ///< No connection to satellites (or not enough).
    DATA_AVAILABLE,    ///< Data is available to be read.
    POSITION_AVAILABLE,///< Position data is available.
    RESTARTING       ///< GPS is restarting.
};

/**
 * @brief Structure holding raw GPS data.
 */
struct GpsRawData
{
    bool status;                 ///< Status of the GPS signal.
    TimeS utc_date_time;         ///< UTC date and time.
    float latitude;              ///< Latitude in degrees.
    float longitude;             ///< Longitude in degrees.
    float msl_m;                 ///< Mean sea level in meters.
    float speed_kph;             ///< Speed in kilometers per hour.
    uint8_t gps_satelites;       ///< Number of GPS satellites in view.
    uint8_t gnss_satelites;      ///< Number of GNSS satellites in view.
    uint8_t glonass_satelites;   ///< Number of GLONASS satellites in view.
    absolute_time_t data_time_stamp; ///< Timestamp of the data.
};

// #-------------------------------#
// | global variables declarations |
// #-------------------------------#

// #-------------------------------#
// | global function declarations  |
// #-------------------------------#

/**
 * @brief Prints the provided time.
 * @param time The time to print.
 */
void time_print(const TimeS& time);

/**
 * @brief Parses a time string and fills the TimeS structure.
 * @param time Reference to a TimeS structure to hold the parsed time.
 * @param str The time string to parse.
 */
void get_time_from_str(TimeS& time, const std::string& str);

/**
 * @brief Namespace for SIM868 GPS module related functions.
 */
namespace sim868
{
    namespace gps
    {
        struct Counters
        {
            struct Responses {
                uint64_t signal; // # received any data with signal
                uint64_t no_signal; // # received any data failed (i.e. no enough data received)
                uint64_t data_available; // # of times that data with valid time received
                uint64_t position_available; // # of times that data with valid position received
                absolute_time_t timestamp_last_response; // last valid response
                absolute_time_t timestamp_last_position; // last valid position
            } response;

            struct Requests {
                uint64_t request_sent; // # of times that used send request to sim868
                uint64_t request_not_sent; // # of times that request could not be sent
                uint64_t response_received; // # of times that sim868 responded
                uint64_t response_not_received; // # of times that sim868 did not respond
            } requests;

            struct Status {
                uint64_t off_on_fetch; // # of times that gps turned off and on to fetch data
                uint64_t restart_on_fetch; // # of times that gps restarted to fetch data
                uint64_t fail_pos_max; // # of times that gps failed to get position max times
                uint64_t fail_pos_curent; // current fail to get position count
            } status;
        };

        /**
         * @brief Turns on the GPS module.
         */
        void turn_on();

        /**
         * @brief Turns off the GPS module.
         */
        void turn_off();

        /**
         * @brief Parses GPS data from a response string.
         * @param respond The response string containing GPS data.
         * @return The parsed GPS raw data.
         */
        GpsRawData get_gps_from_respond(const std::string& respond);

        /**
         * @brief Fetches data from the GPS module.
         * @return True if data was successfully fetched, false otherwise.
         */
        bool fetch_data();

        /**
         * @brief Gets the current speed from the GPS data.
         * @param speed Reference to a float to store the speed.
         * @return True if speed data is available, false otherwise.
         */
        bool get_speed(float& speed);

        /**
         * @brief Gets the current position from the GPS data.
         * @param latitude Reference to a float to store the latitude.
         * @param longitude Reference to a float to store the longitude.
         * @return True if position data is available, false otherwise.
         */
        bool get_position(float& latitude, float& longitude);

        /**
         * @brief Gets the current date and time from the GPS data.
         * @param time Reference to a TimeS structure to store the date and time.
         * @return True if date and time data is available, false otherwise.
         */
        bool get_date(TimeS& time);

        /**
         * @brief Gets the mean sea level altitude from the GPS data.
         * @param msl Reference to a float to store the mean sea level altitude.
         * @return True if MSL data is available, false otherwise.
         */
        bool get_msl(float& msl);

        /**
         * @brief Gets the satellite signal strength from the GPS data.
         * @param sat Reference to a uint8_t to store the number of GPS satellites.
         * @param sat2 Reference to a uint8_t to store the number of GNSS satellites.
         * @return True if signal data is available, false otherwise.
         */
        bool get_signal(uint8_t& sat, uint8_t& sat2);

        /**
         * @brief Gets the current state of the GPS module.
         * @return The current GPS state.
         */
        GpsState get_gps_state(void);

        Counters get_gps_counter(void);
    };
};

#endif // SIM868_GPS_HPP


#ifndef AUTOGEN_TO_STRING_SIM868_GPS_COUNTERS
static inline std::string to_string(const sim868::gps::Counters& obj) {
    std::stringstream ss;
    ss << "sim868::gps::Counters:{";
    ss << " response{";
    ss << " signal=" << obj.response.signal;
    ss << " no_signal=" << obj.response.no_signal;
    ss << " data_available=" << obj.response.data_available;
    ss << " position_available=" << obj.response.position_available;
    ss << " timestamp_last_response=" << obj.response.timestamp_last_response;
    ss << " timestamp_last_position=" << obj.response.timestamp_last_position;
    ss << "}";
    ss << " requests{";
    ss << " request_sent=" << obj.requests.request_sent;
    ss << " request_not_sent=" << obj.requests.request_not_sent;
    ss << " response_received=" << obj.requests.response_received;
    ss << " response_not_received=" << obj.requests.response_not_received;
    ss << "}";
    ss << " status{";
    ss << " off_on_fetch=" << obj.status.off_on_fetch;
    ss << " restart_on_fetch=" << obj.status.restart_on_fetch;
    ss << " fail_pos_max=" << obj.status.fail_pos_max;
    ss << " fail_pos_curent=" << obj.status.fail_pos_curent;
    ss << "}";
    ss << "}";

return ss.str();
}
#endif

#ifndef AUTOGEN_TO_STRING_SIM868_GPS_COUNTERS_YAML
static inline std::string to_string_yaml(const sim868::gps::Counters& obj) {
    std::stringstream ss;
    ss << "Struct {struct_name} contents:\n";
    ss << "- response: " << "\n";
    ss << "    signal:                  " << obj.response.signal << "\n";
    ss << "    no_signal:               " << obj.response.no_signal << "\n";
    ss << "    data_available:          " << obj.response.data_available << "\n";
    ss << "    position_available:      " << obj.response.position_available << "\n";
    ss << "    timestamp_last_response: " << obj.response.timestamp_last_response << "\n";
    ss << "    timestamp_last_position: " << obj.response.timestamp_last_position << "\n";
    ss << "- requests: " << "\n";
    ss << "    request_sent:          " << obj.requests.request_sent << "\n";
    ss << "    request_not_sent:      " << obj.requests.request_not_sent << "\n";
    ss << "    response_received:     " << obj.requests.response_received << "\n";
    ss << "    response_not_received: " << obj.requests.response_not_received << "\n";
    ss << "- status: " << "\n";
    ss << "    off_on_fetch:     " << obj.status.off_on_fetch << "\n";
    ss << "    restart_on_fetch: " << obj.status.restart_on_fetch << "\n";
    ss << "    fail_pos_max:     " << obj.status.fail_pos_max << "\n";
    ss << "    fail_pos_curent:  " << obj.status.fail_pos_curent << "\n";
return ss.str();
}
#endif


















