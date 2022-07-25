#ifndef SIM868_INTERFACE_H
#define SIM868_INTERFACE_H

#include <string>

#define UART_ID0 uart0
#define BAUD_RATE 115200
// #define DATA_BITS 8
// #define STOP_BITS 1
// #define PARITY    UART_PARITY_NONE

#define UART_TX_PIN0 0
#define UART_RX_PIN0 1

#define SIM868_PIN_POWER 14
#define SIM868_PIN_DTR 17



namespace sim868
{
    void init(void);
    void turnOn(void);
    void turnOff(void);
    void reset(void);
    void waitForBoot(void);
    void boot(void);

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
    
    std::string sendRequestGetGpsPos();

}

#endif