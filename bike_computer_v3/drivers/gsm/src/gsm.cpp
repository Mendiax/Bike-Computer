#include "sim868/interface.hpp"
#include "sim868/gsm.hpp"
#include "traces.h"

#include "pico/stdlib.h"


#include <inttypes.h>
#include <iostream>
#include <sstream>


static HttpReadE http_state;
static char* http_respond;

StateStringMachine find_read("+HTTPREAD: ");

extern Response current_response;

void on_uart_rx_http(void)
{
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

    static char str_size[10];
    static uint_fast8_t pos;
    static uint32_t data_size;
    while (uart_is_readable(UART_ID)) 
    {
        const char c = uart_getc(UART_ID);
        switch (http_state)
        {
        case HttpReadE::SENT:
            if(find_read.updateChar(c))
            {
                http_state = HttpReadE::READ_SIZE;
                pos = 0;
            }
            continue;
        case HttpReadE::READ_SIZE:
            if(c != '\r')
            {
                if(c == '\n')
                {
                    str_size[pos++] = '\0';
                    data_size = std::atoi(str_size);
                    pos = 0;
                    http_state = HttpReadE::READING_DATA;
                    http_respond = (char*)malloc(data_size);
                    if(http_respond == NULL)
                    {
                        TRACE_ABNORMAL(TRACE_SIM868, "could not allocate data of size = %" PRIu32 "\n");
                        http_state = HttpReadE::FINISHED;
                        current_response.status = ResponseStatus::FINISH;
                    }
                    TRACE_ABNORMAL(TRACE_SIM868, "allocate data of size = %" PRIu32 "\n");
                    continue;
                }
                str_size[pos++] = c;
            }
            continue;
        case HttpReadE::READING_DATA:
            http_respond[pos++] = c;
            if(pos == data_size)
            {
                http_respond[pos++] = '\0';
                http_state = HttpReadE::FINISHED;
                current_response.status = ResponseStatus::FINISH;
                return;
            }
            continue;
        default:
            break;
        }
    }
}

extern bool http_req_irq;

static GsmStateE current_state = GsmStateE::ON;


uint64_t sim868::gsm::send_request_httpread()
{
    http_req_irq = true;

    http_state = HttpReadE::SENT;
    std::cout << "send_request_httpread" << std::endl;
    return send_request("AT+HTTPREAD", 16000, 0);
}

std::string sim868::gsm::get_respond_httpread(uint64_t id)
{
    if(sim868::check_response(id))
    {
        http_req_irq = false;
        current_response.status = ResponseStatus::RECEIVED; 
        auto resp = std::string(http_respond);
        if(http_respond != NULL)
            free(http_respond);
        return resp;
    }
    return "";
}

bool sim868::gsm::get_signal_strength(uint8_t& rssi, uint8_t& ber)
{
    static uint64_t id;
    if(check_response(id)) // it will return false if id == 0
    {
        auto respond = get_respond(id);
        if(sim868::is_respond_ok(respond))
        {
            auto data_arr = sim868::split_string(respond); // data in arr
            if(data_arr.size() == 2)
            {
                rssi = std::atoi(data_arr[0].c_str());
                ber = std::atoi(data_arr[1].c_str());
            }
            else
            {
                TRACE_ABNORMAL(TRACE_SIM868, "Respond from CSQ is invalid '%s'\n", respond.c_str());
            }
        }
        id = 0;
        return true;
    }
    else if(id == 0)
    {
        id = send_request("AT+CSQ",1000);
    }
    return false;
}

bool sim868::gsm::check_connection(bool& connected)
{
    static uint64_t id;
    if(check_response(id)) // it will return false if id == 0
    {
        auto respond = get_respond(id);
        if(sim868::is_respond_ok(respond))
        {
            sim868::clear_respond(respond);
            connected = (respond == "1\r\n");
        }
        id = 0;
        return true;
    }
    else if(id == 0)
    {
        id = send_request("AT+CGATT?",2000);
    }
    return false;
}

bool sim868::gsm::setup_connection(bool& connected)
{
    const char* at_cmds[] = {
        "AT+CSTT=\"internet\",\"internet\",\"internet\"", // OK or ERROR
        "AT+CIICR", // OK or ERROR
        "AT+CIFSR" // <IP> or ERROR
    };
    enum ConnectionStateE {
        CSTT,
        CIICR,
        CFISR,
        DONE
    };
    static ConnectionStateE state = CSTT;
    static uint64_t id;
    if(check_response(id)) // it will return false if id == 0
    {

        auto respond = get_respond(id);
        //std::cout << "[" << state << "] " << respond << std::endl; 
        id = 0;
        if(!sim868::is_respond_error(respond) && respond.length() > 0)
        {
            // go to next state
            state = (ConnectionStateE)((int)state + 1);
            //printf("next state %d\n", (int)state);

            // TODO extract ip from CIFSR ?
        }
        else
        {
            static uint16_t failed;
            failed++;
            if(failed > 3)
            {
                TRACE_ABNORMAL(TRACE_SIM868 ," Setup failed with state: %d msg: '%s'\n", (int)state, respond.c_str());
                connected = false;
                return true;
            }
        }

        if(state == DONE)
        {
            //printf("done state %d\n", (int)state);
            connected = true;
            return true;
        }
        else
        {
            return false;
        }
    }
    else if(id == 0)
    {
        if(state >= DONE)
        {
            state = CSTT;
        }
        id = send_request(at_cmds[state], 4000);
    }
    return false;
}

bool sim868::gsm::bearer_setup(bool& connected, std::string* ip)
{
    const char* at_cmds[] = {
                // set parameters
                "AT+SAPBR=3,1,\"APN\",\"internet\"", // ok
                "AT+SAPBR=3,1,\"USER\",\"internet\"", // ok
                "AT+SAPBR=3,1,\"PWD\",\"internet\"", // ok
                // open
                "AT+SAPBR=1,1", //ok
                // query
                "AT+SAPBR=2,1", // ret <cid>,<stat>,<ip> ok
    };
    enum ConnectionStateE {
        SET_APN,
        SET_USER,
        SET_PWD,
        OPEN,
        QUERY,
        DONE
    };
    static ConnectionStateE state = SET_APN;
    static uint64_t id;
    if(check_response(id)) // it will return false if id == 0
    {

        auto respond = get_respond(id);
        //std::cout << "[" << state << "] " << respond << std::endl; 
        id = 0;
        if(sim868::is_respond_ok(respond))
        {
            if(state == QUERY && ip != nullptr)
            {
                clear_respond(respond);
                auto params = split_string(respond);
                if(params.size() < 3)
                {
                    TRACE_ABNORMAL(TRACE_SIM868, "SABPR querry not enough args %s\n", respond.c_str());
                }
                else
                {
                    *ip = params[2]; 
                }
            }
            // go to next state
            state = (ConnectionStateE)((int)state + 1);
            //printf("next state %d\n", (int)state);
        }
        else
        {
            static uint16_t failed;
            failed++;
            if(failed > 3)
            {
                TRACE_ABNORMAL(TRACE_SIM868 ,"RB Setup failed with state: %d\n", (int)state);
                connected = false;
                return true;
            }
        }

        if(state == DONE)
        {
            //printf("done state %d\n", (int)state);
            connected = true;
            return true;
        }
        else
        {
            return false;
        }
    }
    else if(id == 0)
    {
        if(state >= DONE)
        {
            state = SET_APN;
        }
        id = send_request(at_cmds[state], 8000);
    }
    return false;
}

bool sim868::gsm::send_http_req(bool& success, const std::string& request, std::string& response, size_t expected_size)
{
    const char* at_cmds[] = {
                // set parameters
                "AT+HTTPINIT", // ok
                "AT+HTTPPARA=\"CID\",1", // ok
                "AT+HTTPPARA=\"URL\",", // ok
                "AT+HTTPACTION=0", // size, resp, ok
                "AT+HTTPREAD", // ok
                "AT+HTTPTERM"
    };
    enum ConnectionStateE {
        INIT,
        SET_CID,
        SET_URL,
        ACTION0,
        READ,
        TERM,
        DONE
    };
    static ConnectionStateE state = INIT;
    static uint64_t id;
    static absolute_time_t action0_time;
    if(check_response(id)) // it will return false if id == 0
    {

        auto respond = get_respond(id);
        std::cout << "[" << state << "] " << respond << std::endl; 
        id = 0;
        if(sim868::is_respond_ok(respond))
        {
            if(state == ACTION0)
            {
                action0_time = get_absolute_time();
            }
            if(state == READ)
            {
                // if(respond)
                //response = respond;
                clear_respond(respond);
                response = respond.substr(respond.find_first_of('\n'));
                //response = std::move(respond);
            }
            // go to next state
            state = (ConnectionStateE)((int)state + 1);
            //printf("next state %d\n", (int)state);
        }
        else
        {
            if(state == ACTION0)
            {
                TRACE_ABNORMAL(TRACE_SIM868 ," ACTION0 failed with state: %d\n", (int)state);
                success = false;
                state = (ConnectionStateE)((int)state + 1);
            }
            else
            {
                TRACE_ABNORMAL(TRACE_SIM868 ," http req failed with state: %d\n", (int)state);
                success = false;
                return true;
            }
        }

        if(state == DONE)
        {
            //printf("done state %d\n", (int)state);
            success = true;
            return true;
        }
        else
        {
            return false;
        }
    }
    else if(id == 0)
    {
        switch (state)
        {
        case SET_URL:
            {
                std::string req_url = at_cmds[state];
                req_url += "\"" + request + "\"";
                id = send_request(req_url.c_str(), 8000);
            }
            break;
        case READ:
            {
                auto current_time = get_absolute_time();
                if(us_to_ms(absolute_time_diff_us(action0_time, current_time)) < 10000)
                {
                    return false; 
                }
                id = send_request(at_cmds[state], 8000 + expected_size);
            }
            break;
        case INIT:
        case SET_CID:
        case ACTION0:
        case TERM:
            id = send_request(at_cmds[state], 8000);
            break;
        case DONE:
        default:
            state = INIT;
            id = send_request(at_cmds[state], 8000);
        }
    }
    return false;  
}

bool sim868::gsm::get_http_req(bool& success, const std::string& request, std::string& response, size_t expected_size)
{
    //std::cout << "get_http_req in [" << (int)current_state << "] " << std::endl;
    if (current_state != GsmStateE::BEARER_OK)
    {
        switch (current_state)
        {
        case GsmStateE::ON:
            {
                bool is_connected = false;
                if(sim868::gsm::check_connection(is_connected))
                {
                    if(is_connected)
                    {
                        current_state = GsmStateE::IS_CONNECTED;
                    }
                    else
                    {
                        // setup failed 
                        // TODO error handling
                    }

                }
            }
            break;
        case GsmStateE::IS_CONNECTED:
            {
                bool is_logged_in = false;
                if(sim868::gsm::setup_connection(is_logged_in))
                {
                    if(is_logged_in)
                    {
                        current_state = GsmStateE::LOGGED_IN;
                    }
                    else
                    {
                        // setup failed 
                        // TODO error handling
                    }

                }
            }
            break;
        case GsmStateE::LOGGED_IN:
            {
                bool bearer_ok = false;
                if(sim868::gsm::bearer_setup(bearer_ok))
                {
                    if(bearer_ok)
                    {
                        current_state = GsmStateE::BEARER_OK;
                    }
                    else
                    {
                        // setup failed 
                        // TODO error handling
                    }
                }
            }
            break;
        default:
            break;
        }
        // setup is not done return false
        return false;
    }
    else
    {
        //std::cout << "get_http_req configured " << std::endl; 
        if(sim868::gsm::send_http_req(success, request, response, 5000))
        {
            if(success == false)
            {
                // TODO error handling ???
                printf("HTTP error\n");
            }
        }
        else
        {
            // waitng for response
            return false;
        }
        return true;
    }
}



