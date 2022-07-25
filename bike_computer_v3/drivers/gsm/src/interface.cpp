#include <string>
#include "sim868/interface.hpp"
#include <pico/stdlib.h>
#include "console/console.h"
//#include "../../../lib2/Config/DEV_GSM_Config.h"
#include <string.h>


class StateStringMachine
{
    const std::string m_stringToFind;
    size_t position;
public:
    StateStringMachine(const std::string&& string)
    :m_stringToFind(string)
    {
        //printf("legth %s: %zu\n",string.c_str(), string.length());
        position = 0;
    }
    bool updateChar(char c)
    {
        if(m_stringToFind[position] == c)
        {
            //printf("__found_char__ %zu\n", position);
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


void sim868::init(void)
{

    // GPIO
    gpio_init(SIM868_PIN_POWER);
    gpio_set_dir(SIM868_PIN_POWER, GPIO_OUT);
    gpio_put(SIM868_PIN_POWER, 1);

    gpio_init(SIM868_PIN_DTR);
    gpio_set_dir(SIM868_PIN_DTR, GPIO_OUT);
    gpio_put(SIM868_PIN_DTR, 1); // TODO idk co to jest xd


    // UART
    uart_init(UART_ID0, BAUD_RATE);
    gpio_set_function(UART_TX_PIN0, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN0, GPIO_FUNC_UART);
}

void sim868::turnOn(void)
{
    gpio_put(SIM868_PIN_POWER, 1);
}

void sim868::turnOff(void)
{
    gpio_put(SIM868_PIN_POWER, 0);
}

// resets module with deleay of 2s
void sim868::reset(void)
{
    turnOff();   
    sleep_ms(2000);
    turnOn();
}

void sim868::boot(void)
{
    init();
    turnOn();
    sleep_ms(5000);
    waitForBoot();
}


// TODO do optimization like:
// send request and wait for specific response (in gps wait for correct number of ',')
std::string sim868::sendRequestLong(const std::string&& cmd, long timeout, const size_t bufferSize)
{
    // TODO do optimization ??
    
    std::string returnString("");
    returnString.reserve(bufferSize);
    StateStringMachine findOk("OK\r\n");
    StateStringMachine findError("ERROR\r\n");

    uint64_t t = time_us_64();
    size_t i = 0;
    consolef("requesting\'%s\'\n", cmd.c_str());
    uart_puts(UART_ID0, cmd.c_str());
    uart_puts(UART_ID0, "\r\n");

    //static char inputBuffer[BUFFER_SIZE];
    while (time_us_64() - t < timeout * 1000)
    {
        while (uart_is_readable_within_us(UART_ID0, 100))
        {
            const char c = uart_getc(UART_ID0);
            returnString += c;
            if(findOk.updateChar(c)) {goto AT_EXIT;}
            if(findError.updateChar(c)) {goto AT_EXIT;}
            // inputBuffer[i++] = uart_getc(UART_ID0);
            // if(i >= BUFFER_SIZE)
            // {
            //     i = BUFFER_SIZE - 1;
            //     goto CHECK;
            // }
        }
    }
//     inputBuffer[i] = '\0';
AT_EXIT:
    returnString.shrink_to_fit();
    consolef("received\'%s\'\n", returnString.c_str());
    return returnString;
}


std::string sim868::sendRequestGetGpsPos()
{
    /*
    'AT+CGNSINF
    +CGNSINF: 1,0,20220723071332.000,,,,0.24,0.0,0,,,,,,8,0,,,42,,

    OK
    '
    */
    constexpr size_t BUFFER_SIZE = 200;
    char inputBuffer[BUFFER_SIZE];
    memset(inputBuffer, 0, BUFFER_SIZE);
    auto cmd = "AT+CGNSINF\r\n";
    consolef("requesting\'%s\'\n", cmd);
    uint64_t t = time_us_64();
    StateStringMachine findOk("OK\r\n");
    StateStringMachine findError("ERROR\r\n");

    long timeout = 2000;
    size_t i = 0;
    size_t commaCount = 0;
    //sleep_ms(2000);
    uart_puts(UART_ID0, cmd);
    while (time_us_64() - t < timeout * 1000)
    {
        while (uart_is_readable_within_us(UART_ID0, timeout))
        {
            const char c = uart_getc(UART_ID0);
            inputBuffer[i++] = c;
            if(findOk.updateChar(c)) {goto GPS_EXIT;}
            if(findError.updateChar(c)) {goto GPS_EXIT;}
            if(i >= BUFFER_SIZE)
            {
                i = BUFFER_SIZE - 1;
                goto GPS_EXIT;
            }
        }
    }
    inputBuffer[i] = '\0';
GPS_EXIT:
    consolef("received\'%s\'\n", inputBuffer);
    return std::string(inputBuffer);
}



void sim868::waitForBoot()
{
    sendRequestLong("ATE1", 2000);
    while(sendRequestLong("AT", 2000).find("OK") == std::string::npos)
    {
        consolep("SIM868 is starting\n");
        reset();
        sleep_ms(2000);
    }
    consolep("SIM868 is ready\n");
}