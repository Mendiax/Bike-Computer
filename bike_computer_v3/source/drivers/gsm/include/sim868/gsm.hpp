#ifndef SIM868_GSM_HPP
#define SIM868_GSM_HPP

#include "sim868/gps.hpp"
#include "utils.hpp"


#include <stdint.h>
#include <sstream>



enum class HttpReadE{
    SENT,
    READ_SIZE,
    READING_DATA,
    FINISHED
};

enum class GsmStateE{
    ON,
    IS_CONNECTED,
    LOGGED_IN,
    BEARER_OK
};

namespace sim868
{
    namespace gsm
    {
        /**
         * @brief Get the signal strength object
         * <rssi>
         *      0           -115 dBm or less
         *      1           -111 dBm 2...30
         *      -110... -54 dBm 31
         *      -52 dBm or greater
         *      99           not known or not detectable
         *
         * <ber>        (in percent):
         *      0...7 As  RXQUAL  values  in  the  table in  GSM 05.08 [20]
         *            subclause 7.2.4
         *      99          Not known or not detectable
         * @param rssi ret
         * @param ber ret
         * @return true if finished
         * @return false
         */
        bool get_signal_strength(uint8_t& rssi, uint8_t& ber);

        /**
         * @brief checks if sim is connected
         *
         * @param connected
         * @return true
         * @return false
         */
        bool check_connection(bool& connected);


        bool setup_connection(bool& connected);

        /**
         * @brief setups RB for connection (cid = 1) (TODO add cid as param ?)
         *
         * @param connected
         * @param ip
         * @return true
         * @return false
         */
        bool bearer_setup(bool& connected, std::string* ip = nullptr); // if null no ip result


        /**
         * @brief sends and receive http request
         *
         * @param success true if http succeded
         * @param request http request
         * @param response response for request
         * @param expected_size expected size of response
         * @return true
         * @return false
         */
        bool send_http_req(bool& success, const std::string& request, std::string& response, size_t expected_size = 4000);

        /**
         * @brief Get the http req do a sim setup if needed
         *
         * @param success true if http succeded
         * @param request http request
         * @param response response for request
         * @param expected_size expected size of response
         * @return true has finished
         * @return false
         */
        bool get_http_req(bool& success, const std::string& request, std::string& response, size_t expected_size = 4000);


        bool setup_gsm(bool& success);


        /**
         * @brief returns url for forecast with given parameters
         *
         * @param latitude
         * @param longitude
         * @param start
         * @param end
         * @return std::string
         */
        template<typename T>
        static inline std::string construct_http_request_url(float latitude, float longitude, const T& start, const T& end)
        {
            std::stringstream url;
            url
            << "http://api.open-meteo.com/v1/forecast?"
            << "latitude=" << latitude << "&" << "longitude=" << longitude
            << "&hourly=temperature_2m,pressure_msl,precipitation,windspeed_10m,winddirection_10m,windgusts_10m&daily=sunrise,sunset,winddirection_10m_dominant&current_weather=true&"
            << "timezone=" << "Europe/Berlin"
            << "&start_date=" << get_iso_format(start)
            << "&end_date=" << get_iso_format(end) ;

            return url.str();

        }

        static inline std::string construct_http_request_url_no_date(float latitude, float longitude)
        {
            std::stringstream url;
            url
            << "http://api.open-meteo.com/v1/forecast?"
            << "latitude=" << latitude << "&" << "longitude=" << longitude
            << "&hourly=temperature_2m,pressure_msl,precipitation,windspeed_10m,winddirection_10m,windgusts_10m&daily=sunrise,sunset,winddirection_10m_dominant&current_weather=true&"
            << "timezone=" << "Europe/Berlin";

            return url.str();
        }
    }
}

#endif