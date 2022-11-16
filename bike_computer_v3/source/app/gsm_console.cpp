#include <stdio.h>
#include <string.h>
//#include <stdarg.h>
#include "pico/stdio_usb.h"
#include <iostream>

#include <pico/stdlib.h>
#include <pico/multicore.h>
#include <pico/sync.h>
#include "hardware/uart.h"

// #define ROTATE
//#include "display/print.h"
// #include "console/console.h"
#include "sim868/interface.hpp"
#include "sim868/gsm.hpp"
// #include "Pico-SIM868-Test.h"
#include "traces.h"

#include "core_utils.hpp"

auto_init_mutex(uart_mutex);

void core1_entry()
{
    size_t i = 0;
    char buffer[500] = {0};
    while (1)
    {
        mutex_enter_blocking(&uart_mutex);
        while (uart_is_readable_within_us(UART_ID, 100 * 1000)) // 100ms
        {
            buffer[i] = uart_getc(UART_ID);
            if (buffer[i] == '\n' || buffer[i] == '\0')
            {
                buffer[i] = '\0';
                printf("%s\n", buffer);
                memset(buffer, 0, sizeof(buffer));
                i = 0;
            }
            else
            {
                i++;
                if(i == 500)
                {
                    printf("[ERROR] overflow\n");
                    buffer[499] = '\0';
                    printf("%s\n", buffer);
                    memset(buffer, 0, sizeof(buffer));
                    i = 0;
                }
            }
        }
        mutex_exit(&uart_mutex);
    }
}

//APN = CMNET
//AT+CGDCONT=2,"IP","internetipv6"

static void gps_test_loop()
{
    while(1)
    {
        static float speed;
        static float latitude;
        static float longitude;
        static float msl;
        static TimeS current_time;
        static uint8_t sat, sat2;
        CYCLE_UPDATE(sim868::gps::fetch_data(), false, 500,
        {
            //TRACE_DEBUG(3,TRACE_CORE_0,
            //            "entering fetch_data\n");
        },
        {
            sim868::gps::get_speed(speed);
            sim868::gps::get_position(latitude, longitude);
            sim868::gps::get_msl(msl);

            time_print(current_time);

            sim868::gps::get_signal(sat,
                                    sat2);

            printf("gps speed %.1f, pos [%.5f,%.5f] date year = %" PRIu16 " signal = [%" PRIu8 ",%" PRIu8 "]\n",
                        speed, latitude, longitude, current_time.year,
                        sat,
                        sat2);
        });
    }
}

static void http_test()
{
    std::string forecast_json;
    std::string http_req_addr = sim868::gsm::construct_http_request_url(52.52, 13.41, (Time_DateS){2022,8,21}, (Time_DateS){2022,8,21});
    //std::string http_req_addr = "http://api.open-meteo.com/v1/forecast?latitude=52.52&longitude=13.41&hourly=temperature_2m";

    bool success;
    while (!sim868::gsm::get_http_req(success, http_req_addr, forecast_json))
    {
        sleep_ms(100);
    }

    std::cout << "forecast: " << forecast_json << std::endl;
}

void consoleGSM()
{
    sim868::boot();

    // sendCMD_waitResp("AT+BTPOWER=1", "OK", 3000);
    // sendCMD_waitResp("AT+BTHOST?", "OK", 3000);
    // sendCMD_waitResp("AT+BTSTATUS?", "OK", 3000);
    // sendCMD_waitResp("AT+BTSCAN=1,10", "OK", 8000);


    sleep_ms(2000);

    // GSM setup
    sim868::sendRequestLong("AT+CSQ",1000); // signal strength


    // http_test();


    // AT+CLBSCFG=0,1
    // AT+CLBSCFG=0,2
    // AT+CLBSCFG=0,3

    // gps_test_loop();


    //http://api.open-meteo.com/v1/forecast?latitude=52.52&longitude=13.41&hourly=temperature_2m

    multicore_launch_core1(core1_entry);

    char buffer[256] = {0};
    while (strcmp(buffer, "exit"))
    {
        scanf("%s", buffer);
        int len = strlen(buffer);
        //printf("len:%d\n", len);
        mutex_enter_blocking(&uart_mutex);
        printf("\nSending to GSM:\"%s\"\n", buffer);
        //printf("\nSending to GSM:\"%s\"\n", buffer);
        uart_puts(UART_ID, buffer);
        uart_puts(UART_ID, "\r\n");
        mutex_exit(&uart_mutex);
    }
    sim868::turnOff();
}


int main(void)
{
    stdio_init_all();
    while (!stdio_usb_connected())
    {
        sleep_ms(100);
    }

    sleep_ms(1000);
    // consoleLogInit();
    sleep_ms(2000);
    tracesSetup();
    TRACES_ON(1, TRACE_SIM868);
    TRACES_ON(3,TRACE_CORE_0);

    consoleGSM();
}