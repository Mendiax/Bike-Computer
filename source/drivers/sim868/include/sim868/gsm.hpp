#ifndef SIM868_GSM_HPP
#define SIM868_GSM_HPP

// #-------------------------------#
// |           includes            |
// #-------------------------------#

// c/c++ includes
#include <stdint.h>
#include <sstream>

// my includes
#include "sim868/gps.hpp"
#include "utils.hpp"

// #-------------------------------#
// |            macros             |
// #-------------------------------#

// #-------------------------------#
// | global types declarations     |
// #-------------------------------#

/**
 * @brief Enumeration for HTTP read states.
 */
enum class HttpReadE {
    SENT,           ///< HTTP request sent.
    READ_SIZE,      ///< Reading size of the response.
    READING_DATA,   ///< Reading response data.
    FINISHED        ///< HTTP read finished.
};

/**
 * @brief Enumeration for GSM states.
 */
enum class GsmStateE {
    ON,             ///< GSM is on.
    IS_CONNECTED,   ///< GSM is connected.
    LOGGED_IN,      ///< GSM is logged in.
    BEARER_OK       ///< Bearer is set up correctly.
};

// #-------------------------------#
// | global variables declarations |
// #-------------------------------#

// #-------------------------------#
// | global function declarations  |
// #-------------------------------#

namespace sim868
{
    namespace gsm
    {
        /**
         * @brief Get the signal strength.
         * @param rssi Reference to store the received signal strength indicator.
         * @param ber Reference to store the bit error rate.
         * @return True if the operation is finished, false otherwise.
         */
        bool get_signal_strength(uint8_t& rssi, uint8_t& ber);

        /**
         * @brief Checks if SIM is connected.
         * @param connected Reference to store the connection status.
         * @return True if the operation is finished, false otherwise.
         */
        bool check_connection(bool& connected);

        /**
         * @brief Sets up the connection.
         * @param connected Reference to store the connection status.
         * @return True if the operation is finished, false otherwise.
         */
        bool setup_connection(bool& connected);

        /**
         * @brief Sets up the bearer for connection.
         * @param connected Reference to store the connection status.
         * @param ip Pointer to store the IP address (optional).
         * @return True if the operation is finished, false otherwise.
         */
        bool bearer_setup(bool& connected, std::string* ip = nullptr);

        /**
         * @brief Sends and receives an HTTP request.
         * @param success Reference to store the success status.
         * @param request The HTTP request string.
         * @param response Reference to store the HTTP response.
         * @param expected_size The expected size of the response.
         * @return True if the operation is finished, false otherwise.
         */
        bool send_http_req(bool& success, const std::string& request, std::string& response, size_t expected_size = 4000);

        /**
         * @brief Gets the HTTP request and performs SIM setup if needed.
         * @param success Reference to store the success status.
         * @param request The HTTP request string.
         * @param response Reference to store the HTTP response.
         * @param expected_size The expected size of the response.
         * @return True if the operation is finished, false otherwise.
         */
        bool get_http_req(bool& success, const std::string& request, std::string& response, size_t expected_size = 4000);

        /**
         * @brief Sets up the GSM.
         * @param success Reference to store the success status.
         * @return True if the operation is finished, false otherwise.
         */
        bool setup_gsm(bool& success);

        /**
         * @brief Constructs a URL for a forecast HTTP request with given parameters.
         * @param latitude Latitude coordinate.
         * @param longitude Longitude coordinate.
         * @param start Start date/time.
         * @param end End date/time.
         * @return Constructed URL string.
         */
        template<typename T>
        static inline std::string construct_http_request_url(float latitude, float longitude, const T& start, const T& end)
        {
            std::stringstream url;
            url << "http://api.open-meteo.com/v1/forecast?"
                << "latitude=" << latitude << "&" << "longitude=" << longitude
                << "&hourly=temperature_2m,pressure_msl,precipitation,windspeed_10m,winddirection_10m,windgusts_10m&daily=sunrise,sunset,winddirection_10m_dominant&current_weather=true&"
                << "timezone=Europe/Berlin"
                << "&start_date=" << get_iso_format(start)
                << "&end_date=" << get_iso_format(end);
            return url.str();
        }

        /**
         * @brief Constructs a URL for a forecast HTTP request without date parameters.
         * @param latitude Latitude coordinate.
         * @param longitude Longitude coordinate.
         * @return Constructed URL string.
         */
        static inline std::string construct_http_request_url_no_date(float latitude, float longitude)
        {
            std::stringstream url;
            url << "http://api.open-meteo.com/v1/forecast?"
                << "latitude=" << latitude << "&" << "longitude=" << longitude
                << "&hourly=temperature_2m,pressure_msl,precipitation,windspeed_10m,winddirection_10m,windgusts_10m&daily=sunrise,sunset,winddirection_10m_dominant&current_weather=true&"
                << "timezone=Europe/Berlin";
            return url.str();
        }
    }
}

#endif // SIM868_GSM_HPP
