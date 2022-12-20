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
#include "traces.h"
#include "massert.hpp"
#include "utils.hpp"

// global

// static variables
static bool sim_booted = false;
static bool sim_on = false;

static std::string rx_buffer;
static volatile bool finished = false;

extern void on_uart_rx_http(void);


Response current_response;

void on_uart_rx(void);

bool sim868::is_booted(void)
{
    return sim_booted;
}


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
    gpio_put(SIM868_PIN_DTR, 1);


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
    sleep_ms(3000);
    waitForBoot();
}

bool sim868::check_for_boot()
{
    static uint64_t id;
    if(check_response(id)) // it will return false if id == 0
    {
        auto respond = get_respond(id);
        sim_booted = sim868::is_respond_ok(respond);
        PRINT("sim boot respond" << respond);
        id = 0;
        return true;
    }
    else if(id == 0)
    {
        id = send_request("AT", 2000);
    }
    return false;
}


bool sim868::check_for_boot_long(void)
{
    return sim868::sendRequestLong("AT", 2000).find("OK") != std::string::npos;
}

void sim868::waitForBoot()
{
    if(!sim_on)
    {
        TRACE_ABNORMAL(TRACE_SIM868, "sim868 is off, cannot boot\n");
        return;
    }
    int fail_counter = 0;
    while(!check_for_boot_long())
    {
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
            TRACE_ABNORMAL(TRACE_SIM868, "not enough params %s, %zu\n", respond.c_str(), values.size());
            return false;
        }

        is_charging = std::atoi(values[0].c_str());
        bat_lev = std::atoi(values[1].c_str());
        voltage = std::atoi(values[2].c_str());
        return true;
    }
    else if(id == 0)
    {
        // sending response
        id = send_request("AT+CBC",2000);
    }
    else
    {
        // still waits for response
    }
    return false;
}


void on_uart_rx(void)
{
    static StateStringMachine find_ok("\r\nOK\r\n");
    static StateStringMachine find_error("\r\nERROR\r\n");
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
            current_response.status = ResponseStatus::RECEIVED;
            return;
        }
    }
}


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

    // prepare
    current_response.response.clear();
    current_response.response.reserve(bufferSize);
    current_response.time_start = get_absolute_time();
    current_response.status = ResponseStatus::SENT;
    current_response.timeout = timeout;
    current_response.id = id;

    TRACE_DEBUG(2, TRACE_SIM868, "new request \'%s\' %" PRIu64 "\n", cmd.c_str(), id);
    // send request
    uart_puts(UART_ID, cmd.c_str());
    uart_puts(UART_ID, "\r\n");

    #ifdef GSM_WAIT
    while(check_response(id))
    {
        sleep_ms(10);
    }
    #endif
    return id;
}

std::string sim868::get_respond(uint64_t id)
{
    if(sim868::check_response(id))
    {
        if(is_respond_error(current_response.response))
        {
            TRACE_ABNORMAL(TRACE_SIM868, "received error '%s'\n", current_response.response.c_str());
        }
        current_response.status = ResponseStatus::RECEIVED;
        return current_response.response;
    }
    return "";
}

bool sim868::check_response(uint64_t id)
{
    if(current_response.id == id && id != 0)
    {
        if(current_response.status != ResponseStatus::RECEIVED &&
            us_to_ms(absolute_time_diff_us(current_response.time_start, get_absolute_time())) > current_response.timeout)
        {
            // timeout has occured
            current_response.status = ResponseStatus::TIME_OUT;
        }
        switch (current_response.status)
        {
            // those are cases when we should not received any data so we remove from buffer
            case ResponseStatus::TIME_OUT:
            case ResponseStatus::RECEIVED:
                return true;
        }
    }
    return false;
}


std::string sim868::sendRequestLong(const std::string&& cmd, long timeout, const size_t bufferSize)
{
    std::string resp;
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
    // std::cout << resp << std::endl;
    return resp;
}





bool sim868::is_respond_ok(const std::string& respond)
{
    auto ok_msg = "\r\nOK\r\n";
    auto ok_pos = respond.length() - strlen(ok_msg);
    // PRINTF("%u -> '%s'\n",ok_pos, &respond.c_str()[ok_pos]);
    return strcmp(&respond.c_str()[ok_pos], ok_msg) == 0;
}

bool sim868::is_respond_error(const std::string& respond)
{
    auto ok_msg = "\r\nERROR\r\n";
    auto ok_pos = respond.length() - strlen(ok_msg);
    // PRINTF("%u -> '%s'\n",ok_pos, &respond.c_str()[ok_pos]);
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

    if(respond.at(last_char_idx) == '\r')
    {
        --last_char_idx;
    }
    if(respond.at(last_char_idx) == '\n')
    {
        --last_char_idx;
    }

    if(nl_cnt != 0 || last_char_idx == 0)
    {
        // not found OK or ERROR
        last_char_idx = str_length - 1;
    }

    // cut to correct len
    respond = respond.substr(cmd_start_idx, last_char_idx - cmd_start_idx);
}

