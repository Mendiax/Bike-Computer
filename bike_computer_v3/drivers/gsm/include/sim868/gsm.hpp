#ifndef SIM868_GSM_HPP
#define SIM868_GSM_HPP

#include "sim868/gps.hpp"

#include <stdint.h>


enum class HttpReadE{
    SENT,
    READ_SIZE,
    READING_DATA,
    FINISHED
};

namespace sim868
{
    namespace gsm
    {
        // deprecated
        uint64_t send_request_httpread();
        std::string get_respond_httpread(uint64_t id);

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

    }
}

#endif