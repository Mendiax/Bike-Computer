#ifndef SIM868_INTERFACE_H
#define SIM868_INTERFACE_H

// #-------------------------------#
// |           includes            |
// #-------------------------------#

#include <string>
#include <vector>
#include <stdint.h>
#include <pico/stdlib.h>
#include "traces.h"

// #-------------------------------#
// |            macros             |
// #-------------------------------#

#define UART_ID uart0
#define UART_IRQ UART0_IRQ
#define BAUD_RATE 115200

#define UART_TX_PIN0 0
#define UART_RX_PIN0 1

#define SIM868_PIN_POWER 14
#define SIM868_PIN_DTR 17

#define GSM_WAIT

// #-------------------------------#
// | local class definitions       |
// #-------------------------------#

/**
 * @brief Class for managing state of a string search using the KMP algorithm.
 */
class StateStringMachine
{
    const std::string stringToFind;
    std::vector<size_t> prefix;
    size_t m;
    size_t q = 0;

    /**
     * @brief Computes the prefix function for the KMP algorithm.
     * @param pattern The pattern string to search for.
     * @return The prefix function as a vector of sizes.
     */
    std::vector<size_t> compute_prefix_function(const std::string pattern) {
        size_t m = pattern.length();
        std::vector<size_t> pi;
        pi.resize(m);
        pi[1] = 0;
        size_t k = 0;
        for(size_t q = 1; q < m; q++){
            while (k > 0 && pattern[k] != pattern[q]){
                k = pi[k];
            }
            if (pattern[k] == pattern[q])
                k++;
            pi[q] = k;
        }
        return pi;
    }

public:
    /**
     * @brief Constructor initializing the string to find.
     * @param string The string to search for.
     */
    StateStringMachine(std::string string)
    :stringToFind(string)
    {
        prefix = compute_prefix_function(string);
        m = string.length();
    }

    /**
     * @brief Updates the state machine with a new character.
     * @param c The new character to process.
     * @return True if the full string was found, false otherwise.
     */
    bool updateChar(char c)
    {
        while (q > 0 && stringToFind[q] != c)
            q = prefix[q-1];
        if(stringToFind[q] == c)
            q++;
        if(q == m){
            q = prefix[q-1];
            return true;
        }
        return false;
    }
};

// #-------------------------------#
// | global types declarations     |
// #-------------------------------#

/**
 * @brief Enumeration for response status.
 */
enum class ResponseStatus
{
    SENT,        ///< Request sent.
    STARTED,     ///< Request started.
    TIME_OUT,    ///< Request timed out.
    RECEIVED     ///< Response received.
};

/**
 * @brief Structure representing a response.
 */
struct Response
{
    std::string response;          ///< Response string.
    volatile ResponseStatus status;///< Status of the response.
    absolute_time_t time_start;    ///< Start time of the response.
    uint64_t id;                   ///< ID of the response.
    long timeout;                  ///< Timeout duration for the response.
};

// #-------------------------------#
// | global function declarations  |
// #-------------------------------#

namespace sim868
{
    bool is_booted(void);
    bool is_on(void);
    void init(void);
    void turn_on(void);
    void turn_off(void);
    void reset(void);
    void waitForBoot(void);

    bool check_for_boot(void);
    bool check_for_boot_long(void);

    /**
     * @brief Initializes, turns on, and waits for boot to finish.
     */
    void boot(void);

    /**
     * @brief Checks the battery level.
     * @param is_charging Reference to store the charging status.
     * @param bat_lev Reference to store the battery level in percentage.
     * @param voltage Reference to store the battery voltage.
     * @return True if the command received a response, false otherwise.
     */
    bool get_bat_level(bool& is_charging,
                       uint8_t& bat_lev,
                       uint16_t& voltage);

    /**
     * @brief Sends a request to SIM868 with a flexible buffer.
     * Used for commands that can take a large amount of space or have uncertain size.
     * @param cmd The command string to send.
     * @param timeout The timeout duration.
     * @param bufferSize The initial size of the buffer.
     * @return The response string.
     */
    std::string sendRequestLong(const std::string&& cmd,
                                long timeout,
                                size_t bufferSize = 100);

    /**
     * @brief Sends a request to SIM868 with a flexible buffer without waiting for response.
     * @param cmd The command string to send.
     * @param timeout The timeout duration.
     * @param bufferSize The initial size of the buffer.
     * @return The request ID.
     */
    uint64_t send_request(const std::string&& cmd,
                          long timeout,
                          size_t bufferSize = 100);

    /**
     * @brief Checks if the sent request has finished by timeout or received full response.
     * @param id The request ID to check.
     * @return True if the request has finished, false otherwise.
     */
    bool check_response(uint64_t id);

    /**
     * @brief Gets the response from the last request.
     * @param id The request ID to get the response for.
     * @return The response string.
     */
    std::string get_respond(uint64_t id);

    /**
     * @brief Checks if the response ends with "\r\nOK\r\n".
     * @param respond The response string to check.
     * @return True if the response ends with "\r\nOK\r\n", false otherwise.
     */
    bool is_respond_ok(const std::string& respond);

    /**
     * @brief Checks if the response ends with "\r\nERROR\r\n".
     * @param respond The response string to check.
     * @return True if the response ends with "\r\nERROR\r\n", false otherwise.
     */
    bool is_respond_error(const std::string& respond);

    /**
     * @brief Clears the response from unnecessary data.
     * Assumes that data starts with command and ends with \r\nOK\r\n or \r\nERROR\r\n.
     * @param respond The raw response string to clear.
     */
    void clear_respond(std::string& respond);
}

#endif // SIM868_INTERFACE_H
