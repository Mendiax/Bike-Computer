#ifndef SIM868_INTERFACE_H
#define SIM868_INTERFACE_H

#include <string>
#include <vector>
#include <stdint.h>
#include "hardware/uart.h"
#include <pico/stdlib.h>



#define UART_ID uart0
#define UART_IRQ UART0_IRQ
#define BAUD_RATE 115200
// #define DATA_BITS 8
// #define STOP_BITS 1
// #define PARITY    UART_PARITY_NONE

#define UART_TX_PIN0 0
#define UART_RX_PIN0 1

#define SIM868_PIN_POWER 14
#define SIM868_PIN_DTR 17

#define GSM_WAIT

// #----------------------------#
// local class definitons 
// #----------------------------#
class StateStringMachine
{
    const std::string m_stringToFind;
    size_t position;
public:
    StateStringMachine(const std::string&& string)
    :m_stringToFind(string)
    {
        //PRINTF("legth %s: %zu\n",string.c_str(), string.length());
        position = 0;
    }
    bool updateChar(char c)
    {
        if(m_stringToFind[position] == c)
        {
            //PRINTF("__found_char__ %zu\n", position);
            position++;
            if(position == m_stringToFind.length())
            {
                return true;
            }
        }
        else
        {
            position = 0;
        }
        return false;
    }
};



enum class ResponseStatus
{
    SENT,
    STARTED,
    FINISH,
    TIME_OUT,
    RECEIVED
};

struct Response
{
    std::string response;
    volatile ResponseStatus status;
    absolute_time_t time_start;
    uint64_t id;
    long timeout; 
};

namespace sim868
{
    bool is_on(void);
    void init(void);
    void turnOn(void);
    void turnOff(void);
    void reset(void);
    void waitForBoot(void);

    bool check_for_boot(void);

    /**
     * @brief initialize, turn on and wait for boot finish
     * 
     */
    void boot(void);
    
    //std::string sendRequestGetGpsPos();

    /**
     * @brief check battery level
     * returns is [<charging>, <battery level in %>, <voltage>]
     * 
     * @return true if cmd got respond
     */
    bool get_bat_level(bool& is_charging,
                       uint8_t& bat_lev,
                       uint16_t& voltage);

    /**
     * @brief Function for sending request to SIM868 with flexible buffer
     * It should be used with commands that can take big ammount of space
     * or we are not sure how much it will take
     * 
     * @param cmd 
     * @param timeout 
     * @param bufferSize initial size of buffer
     * @return std::string 
     */
    std::string sendRequestLong(const std::string&& cmd,
                                long timeout,
                                size_t bufferSize = 100
                                );

    uint64_t send_request_httpread();


    /**
     * @brief Function for sending request to SIM868 with flexible buffer
     * It should be used with commands that can take big ammount of space
     * or we are not sure how much it will take. It will not wait for
     * response
     * 
     * @param cmd 
     * @param timeout 
     * @param bufferSize initial size of buffer
     * @return request id
     */
    uint64_t send_request(const std::string&& cmd,
                          long timeout,
                          size_t bufferSize = 100
                          );
    /**
     * @brief Check if sent request has finished by time out or recived full response
     * 
     * @return true sent request has finished by time out or recived full response
     * @return false sent request is waiting for response
     */
    bool check_response(uint64_t id);

    // /**
    //  * @brief reutrns current response status
    //  * 
    //  * @return ResponseStatus 
    //  */
    // ResponseStatus check_response_status();

    /**
     * @brief Get the respond from last request
     * 
     * @return std::string of respond
     */
    std::string get_respond(uint64_t id);

    /**
     * @brief check if end of msg is "\r\nOK\r\n"
     * 
     * @param respond 
     * @return true 
     * @return false 
     */
    bool is_respond_ok(const std::string& respond);
    bool is_respond_error(const std::string& respond);



    /**
     * @brief clears respond from not needed data
     * This assumes that data start with cmd and ends with \r\nOK\r\n or \rn\ERROR\r\n
     * 
     * @param respond string of raw respond
     */
    void clear_respond(std::string& respond);

    /**
     * @brief splits given string by given sepparator
     * 
     * @param string 
     * @param sep 
     * @return std::vector<std::string> 
     */
    std::vector<std::string> split_string(const std::string& string, char sep = ',');
}

#endif