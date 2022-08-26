// pico includes
#include <pico/stdlib.h>

// c/c++ includes
#include <string.h>
#include <string>
#include <inttypes.h>
#include <tuple>
#include <vector>
#include <iostream>

// my includes
#include "sim868/interface.hpp"
#include "console/console.h"
#include "traces.h"
#include "massert.h"

// static variables
bool gps_on;
bool sim_on;
bool waiting_for_request_respond;
volatile bool http_req_irq;

static std::string rx_buffer;
static volatile bool finished = false;

extern void on_uart_rx_http(void);


Response current_response;

void on_uart_rx(void);

bool sim868::is_on(void)
{
    return sim_on;
}

void sim868::init(void)
{
    // init current_response
    current_response.status = ResponseStatus::RECEIVED;
    current_response.response = "";

    // GPIO
    gpio_init(SIM868_PIN_POWER);
    gpio_set_dir(SIM868_PIN_POWER, GPIO_OUT);
    gpio_put(SIM868_PIN_POWER, 1);

    gpio_init(SIM868_PIN_DTR);
    gpio_set_dir(SIM868_PIN_DTR, GPIO_OUT);
    gpio_put(SIM868_PIN_DTR, 1); // TODO idk co to jest xd


    // UART
    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(UART_TX_PIN0, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN0, GPIO_FUNC_UART);


    // set irq on recive
    uart_set_fifo_enabled(UART_ID, true);
    // Set up a RX interrupt
    // We need to set up the handler first

    // And set up and enable the interrupt handlers
    irq_set_exclusive_handler(UART_IRQ, on_uart_rx);
    irq_set_enabled(UART_IRQ, true);

    // Now enable the UART to send interrupts - RX only
    uart_set_irq_enables(UART_ID, true, false);
}

void sim868::turnOn(void)
{
    gpio_put(SIM868_PIN_POWER, 1);
    sim_on = 1;
    current_response.status = ResponseStatus::RECEIVED;
    current_response.response = "";
}

void sim868::turnOff(void)
{
    gpio_put(SIM868_PIN_POWER, 0);
    sim_on = 0;
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

bool sim868::check_for_boot(void)
{
    return sim868::sendRequestLong("AT", 2000).find("OK") != std::string::npos;
}


// std::string sim868::sendRequestGetGpsPos()
// {
//     /*
//     'AT+CGNSINF
//     +CGNSINF: 1,0,20220723071332.000,,,,0.24,0.0,0,,,,,,8,0,,,42,,

//     OK
//     '
//     */
//     constexpr size_t BUFFER_SIZE = 200;
//     char inputBuffer[BUFFER_SIZE];
//     memset(inputBuffer, 0, BUFFER_SIZE);
//     auto cmd = "AT+CGNSINF\r\n";
//     consolef("requesting\'%s\'\n", cmd);
//     uint64_t t = time_us_64();
//     StateStringMachine findOk("OK\r\n");
//     StateStringMachine findError("ERROR\r\n");

//     long timeout = 2000;
//     size_t i = 0;
//     size_t commaCount = 0;
//     //sleep_ms(2000);
//     uart_puts(UART_ID0, cmd);
//     while (time_us_64() - t < timeout * 1000)
//     {
//         while (uart_is_readable_within_us(UART_ID0, timeout))
//         {
//             const char c = uart_getc(UART_ID0);
//             inputBuffer[i++] = c;
//             if(findOk.updateChar(c)) {goto GPS_EXIT;}
//             if(findError.updateChar(c)) {goto GPS_EXIT;}
//             if(i >= BUFFER_SIZE)
//             {
//                 i = BUFFER_SIZE - 1;
//                 goto GPS_EXIT;
//             }
//         }
//     }
//     inputBuffer[i] = '\0';
// GPS_EXIT:
//     consolef("received\'%s\'\n", inputBuffer);
//     return std::string(inputBuffer);
// }




void sim868::waitForBoot()
{
    if(!sim_on)
    {
        TRACE_ABNORMAL(TRACE_SIM868, "sim868 is off, cannot boot\n");
        return;
    }
    int fail_counter = 0;
    while(!check_for_boot())
    {
        consolep("SIM868 is starting\n");
        sleep_ms(1000);
        fail_counter++;
        if(fail_counter > 5)
        {
            TRACE_ABNORMAL(TRACE_SIM868, "sim868 start has failed %d times\n", fail_counter);
            reset();
        }
        if(fail_counter > 10)
        {
            turnOff();
            return;
        }
    }
    consolep("SIM868 is ready\n");
}

 

bool sim868::get_bat_level(bool& is_charging,
                           uint8_t& bat_lev,
                           uint16_t& voltage)
{
    static uint64_t id;
    if(check_response(id)) // it will return false if id == 0
    {
        // response received
        auto respond = get_respond(id);
        id = 0;
        clear_respond(respond);
        //std::cout << respond << std::endl;
        auto values = split_string(respond);
        if(values.size() < 3)
        {
            TRACE_ABNORMAL(TRACE_SIM868, "not enough params %zu\n", values.size());
            return false;
        }

        // TODO add debug ind
        // std::cout << values[0] << std::endl;
        // std::cout << values[1] << std::endl;
        // std::cout << values[2] << std::endl;

        is_charging = std::atoi(values[0].c_str());
        bat_lev = std::atoi(values[1].c_str());
        voltage = std::atoi(values[2].c_str());
        return true;
    }
    else if(id == 0)
    {
        // sending response
        id = send_request("AT+CBC",100);
    }
    else
    {
        // still waits for response
    }
    //TRACE_ABNORMAL(TRACE_SIM868, "req\n");
    //auto respond = sendRequestLong("AT+CBC",100);
    return false;
}

StateStringMachine find_ok("OK\r\n");
StateStringMachine find_error("ERROR\r\n");
void on_uart_rx(void)
{
    if(http_req_irq)
    {
        on_uart_rx_http();
        return;
    }
    switch (current_response.status)
    {
    case ResponseStatus::SENT:
        current_response.status = ResponseStatus::STARTED; 
        break;
    case ResponseStatus::STARTED:
        if(absolute_time_diff_us(current_response.time_start, get_absolute_time()) > current_response.timeout * 1000)
        {
            // timeout has occured
            current_response.status = ResponseStatus::TIME_OUT;
            return;
        }
        break;
    // those are cases when we should not received any data so we remove from buffer
    case ResponseStatus::FINISH:
    case ResponseStatus::TIME_OUT:
    case ResponseStatus::RECEIVED:
        // read data in buffer
        while (uart_is_readable(UART_ID)) 
        {
            uart_getc(UART_ID);
        }
        return;
    default:
        break;
    }

    while (uart_is_readable(UART_ID)) 
    {
        const char c = uart_getc(UART_ID);
        current_response.response += c;
        if(find_ok.updateChar(c) || find_error.updateChar(c))
        {
            // we have finished receive data
            current_response.status = ResponseStatus::FINISH;
            return;
        }
    }
}


// TODO add queue
uint64_t sim868::send_request(const std::string&& cmd,
                              long timeout,
                              size_t bufferSize
                              )
{
    static uint64_t id;
    if(id == 0){++id;} // proper id cannot be 0 
    if(!is_on())
    {
        TRACE_ABNORMAL(TRACE_SIM868, "sim868 is off, cannot send request\n");
        return 0;
    }

    if(current_response.status != ResponseStatus::RECEIVED)
    {
        // there is request pending so it needs to be received first
        TRACE_ABNORMAL(TRACE_SIM868, "there is pending request, cannot send request '%s'\n", cmd.c_str());
        return 0;
    }
    ++id;
    // TODO do optimization ??
    //TRACE_DEBUG(1,TRACE_SIM868,"requesting\'%s\'\n", cmd.c_str());

    // prepare
    current_response.response.clear();
    current_response.response.reserve(bufferSize);
    current_response.time_start = get_absolute_time();
    current_response.status = ResponseStatus::SENT;
    current_response.timeout = timeout;
    current_response.id = id;

    // send request
    uart_puts(UART_ID, cmd.c_str());
    uart_puts(UART_ID, "\r\n");
    
    TRACE_DEBUG(2, TRACE_SIM868, "new request \'%s\' %" PRIu64 "\n", cmd.c_str(), id);
    return id;
}

std::string sim868::get_respond(uint64_t id)
{
    if(sim868::check_response(id))
    {
        //std::cout << id << " -> " << current_response.response << std::endl;
        current_response.status = ResponseStatus::RECEIVED; 
        return current_response.response; 
    }
    return "";
}

bool sim868::check_response(uint64_t id)
{
    if(current_response.id == id && id != 0)
    {
        if(us_to_ms(absolute_time_diff_us(current_response.time_start, get_absolute_time())) > current_response.timeout)
        {
            // timeout has occured
            current_response.status = ResponseStatus::TIME_OUT;
        }
        switch (current_response.status)
        {
            // those are cases when we should not received any data so we remove from buffer
            case ResponseStatus::FINISH:
            case ResponseStatus::TIME_OUT:
            case ResponseStatus::RECEIVED:
                return true;
        }
    }
    return false;
}


// TODO do optimization like:
// send request and wait for specific response (in gps wait for correct number of ',')
std::string sim868::sendRequestLong(const std::string&& cmd, long timeout, const size_t bufferSize)
{
    std::string resp;
    // std::string cmd_ = cmd; 
    auto req = sim868::send_request(cmd.c_str(), timeout, bufferSize);
    if(req == 0)
    {
        return "";
    }
    while (!check_response(req))
    {
        sleep_ms(100);
    }
    resp = get_respond(req);
    std::cout << resp << std::endl;
    return resp;
    
    //     if(!is_on())
    //     {
    //         TRACE_ABNORMAL(TRACE_SIM868, "sim868 is off, cannot send request\n");
    //         return "";
    //     }
    //     // TODO do optimization ??
        
    //     std::string returnString("");
    //     returnString.reserve(bufferSize);
    //     StateStringMachine findOk("OK\r\n");
    //     StateStringMachine findError("ERROR\r\n");

    //     TRACE_DEBUG(1,TRACE_SIM868,"requesting\'%s\'\n", cmd.c_str());
    //     uint64_t t = time_us_64();
    //     size_t i = 0;
        
    //     rx_buffer = "";

    //     uart_puts(UART_ID, cmd.c_str());
    //     uart_puts(UART_ID, "\r\n");

    //     #if 1

    //     while (time_us_64() - t < timeout * 1000)
    //     {
    //         sleep_ms(1);
    //     }

    //     returnString = rx_buffer;


    //     #else

    //     //static char inputBuffer[BUFFER_SIZE];
    //     while (time_us_64() - t < timeout * 1000)
    //     {
    //         while (uart_is_readable_within_us(UART_ID, 100))
    //         {
    //             const char c = uart_getc(UART_ID);
    //             returnString += c;
    //              // TRACE_ABNORMAL(TRACE_SIM868, "read char '%c'\n", c);
    //             if(findOk.updateChar(c)) {goto AT_EXIT;}
    //             if(findError.updateChar(c)) {goto AT_EXIT;}
    //             // inputBuffer[i++] = uart_getc(UART_ID0);
    //             // if(i >= BUFFER_SIZE)
    //             // {
    //             //     i = BUFFER_SIZE - 1;
    //             //     goto CHECK;
    //             // }
    //         }
    //     }
    //     #endif
    // //     inputBuffer[i] = '\0';
    // AT_EXIT:
    //     returnString.shrink_to_fit();
    //     TRACE_DEBUG(1,TRACE_SIM868,"received\'%s\'\n", returnString.c_str());
    //     return returnString;
}





bool sim868::is_respond_ok(const std::string& respond)
{
    auto ok_msg = "\r\nOK\r\n";
    auto ok_pos = respond.length() - strlen(ok_msg);
    // printf("%u -> '%s'\n",ok_pos, &respond.c_str()[ok_pos]);
    return strcmp(&respond.c_str()[ok_pos], ok_msg) == 0;
}

bool sim868::is_respond_error(const std::string& respond)
{
    auto ok_msg = "\r\nERROR\r\n";
    auto ok_pos = respond.length() - strlen(ok_msg);
    // printf("%u -> '%s'\n",ok_pos, &respond.c_str()[ok_pos]);
    return strcmp(&respond.c_str()[ok_pos], ok_msg) == 0;
}


void sim868::clear_respond(std::string& respond)
{
    // example of cmd AT+CBC
    // 'AT+CBC
    // +CBC: 0,70,3961\r\n
    // \r\n
    // OK\r\n
    // '
    auto str_length = respond.length();
    if(str_length == 0)
    {
        TRACE_ABNORMAL(TRACE_SIM868, "Respond is null\n");
        return;
    }

    // idx of start
    auto cmd_start_idx = respond.find(':');
    if(cmd_start_idx == std::string::npos)
    {
        TRACE_ABNORMAL(TRACE_SIM868, "could not find ':' in %s\n", respond.c_str());
        return;
    }
    if(respond.at(cmd_start_idx) == ':')
        cmd_start_idx += 1;
    if(respond.at(cmd_start_idx) == ' ')
        cmd_start_idx += 1;
    // if(isspace(respond.at(cmd_start_idx)))
    // {
    //     cmd_start_idx++;
    // }

    auto last_char_idx = str_length - 1;
    int_fast8_t nl_cnt = 2; 
    while (last_char_idx > 0 && nl_cnt > 0)
    {
        if(respond.at(last_char_idx) == '\r')
        {
            --nl_cnt;
        }
        --last_char_idx;
    }
    if(nl_cnt == 0)
    {
        last_char_idx++;
    }

    if(nl_cnt != 0 || last_char_idx == 0)
    {
        // not found OK or ERROR
        last_char_idx = str_length - 1;
    }

    // cut to correct len
    respond = respond.substr(cmd_start_idx, last_char_idx - cmd_start_idx);

    // if(str_length > strlen("\r\nOK\r\n"))
    // {
    //     // check for OK
    // }
    // if(str_length > strlen("\r\nERROR\r\n"))
    // {
    //     // check for ERROR
    // }
}

std::vector<std::string> sim868::split_string(const std::string& string, char sep)
{
    std::vector<std::string> strings; // this will be returned

    size_t begin_idx, end_idx;
    begin_idx = 0;
    while((end_idx = string.find(sep, begin_idx)) != std::string::npos)
    {
        //end_idx = string.find(sep, begin_idx);
        strings.emplace_back(string.substr(begin_idx, end_idx - begin_idx));
        begin_idx = end_idx + 1;
    }
    if(begin_idx < string.length())
        strings.emplace_back(string.substr(begin_idx));
    return strings;
}

