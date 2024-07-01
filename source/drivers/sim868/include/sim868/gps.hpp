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
    DATA_AVAIBLE,    ///< Data is available to be read.
    POSITION_AVAIBLE,///< Position data is available.
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
    };
};

#endif // SIM868_GPS_HPP
