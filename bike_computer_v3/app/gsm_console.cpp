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
#include "console/console.h"
#include "sim868/interface.hpp"
#include "sim868/gsm.hpp"
// #include "Pico-SIM868-Test.h"
#include "traces.h"

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

// void singleThreadConsole()
// {
//     DEV_GSM_Module_Init();
//     powerOn;
//     led_blink();
//     DEV_GSM_Delay_ms(5000);
//     check_start();
//     //started
//     consolep("console started\n");

//     char consoleBuffer[1024] = {0};
//     size_t posConsole = 0;
//     char gsmBuffer[1024] = {0};
//     size_t posGsm = 0;

//     while (1)
//     {
//         int c;
//         while((c = getchar_timeout_us(1 * 1000)) != STDIO_NO_INPUT)
//         {
//             //handle input from pc
//             consoleBuffer[posConsole] = (char) c;
//             if (consoleBuffer[posConsole] == '\n' || consoleBuffer[posConsole] == '\0')
//             {
//                 consoleBuffer[posConsole] = '\0';
//                 consolef("%s", consoleBuffer);
//                 memset(buffer, 0, sizeof(buffer));
//                 posConsole = 0;
//             }
//             else
//             {
//                 posConsole++;
//             }
//         }
//     }
    
//     powerDown;
// }

//APN = CMNET
//AT+CGDCONT=2,"IP","internetipv6"

void consoleGSM()
{
    // DEV_GSM_Module_Init();
    // powerOn;
    // led_blink();
    // DEV_GSM_Delay_ms(5000);

    // check_start();

    sim868::boot();

    // sendCMD_waitResp("AT+BTPOWER=1", "OK", 3000);
    // sendCMD_waitResp("AT+BTHOST?", "OK", 3000);
    // sendCMD_waitResp("AT+BTSTATUS?", "OK", 3000);
    // sendCMD_waitResp("AT+BTSCAN=1,10", "OK", 8000);
    



    //AT+CSTT="internet","internet","internet"
    // AT+CIPPING="www.google.com"
    sleep_ms(2000);

    // GSM setup
    sim868::sendRequestLong("AT+CSQ",1000); // signal strength
    // std::string cgatt = sim868::sendRequestLong("AT+CGATT?", 1000);
    // sim868::clear_respond(cgatt);
    // if(cgatt!="1\r\n")
    // {
    //     std::cout << "cgatt '" << cgatt << "'" << std::endl;
    //     do
    //     {
    //         sleep_ms(1000);
    //         cgatt = sim868::sendRequestLong("AT+CGATT?", 1000);
    //         sim868::clear_respond(cgatt);
    //     } while (cgatt!="1\r\n"); // wait for connection
        
    // }
    // sleep_ms(2000);

    // TODO coś zepsute w setupie xd help

    bool connected = false;
    while(!connected)
    {
        sim868::gsm::check_connection(connected);
        sleep_ms(1000);
    }

    // // setup connection
    // sim868::sendRequestLong("AT+CSTT=\"internet\",\"internet\",\"internet\"", 8000);
    // //should be CGDCONT: 1,"IP","internet","0.0.0.0",0,0
    // sim868::sendRequestLong("AT+CIICR", 1000);
    // sim868::sendRequestLong("AT+CIFSR", 1000);
    connected = false;
    while(!connected)
    {
        if(sim868::gsm::setup_connection(connected))
        {
            if(connected == false)
            {
                printf("ERROR CSTT\n");
                sleep_ms(10*1000);
            }
        }
        sleep_ms(100);
    }

    // optional test
    //sim868::sendRequestLong("AT+CIPPING=\"www.google.com\"", 8000);
    //sleep_ms(1000); // ????????????????????

    // // bearer config
    // setup for cid 1
    // set parameters
    // sim868::sendRequestLong("AT+SAPBR=3,1,\"APN\",\"internet\"", 2000); // ok
    // sim868::sendRequestLong("AT+SAPBR=3,1,\"USER\",\"internet\"", 2000); // ok
    // sim868::sendRequestLong("AT+SAPBR=3,1,\"PWD\",\"internet\"", 2000); // ok
    // // open
    // sim868::sendRequestLong("AT+SAPBR=1,1",2000); //ok
    // // query
    // sim868::sendRequestLong("AT+SAPBR=2,1",2000); // ret <cid>,<stat>,<ip>

    connected = false;
    std::string ip;
    while(!connected)
    {
        if(sim868::gsm::bearer_setup(connected, &ip))
        {
            if(connected == false)
            {
                printf("ERROR BEARER\n");
                sleep_ms(10*1000);
            }
        }
        sleep_ms(100);
    }
    std::cout << "extracted ip " << ip << std::endl;

    sim868::sendRequestLong("AT+CSQ",1000); // signal strength

    sleep_ms(5000);

    bool success = false;
    std::string forecast_json;
    while(!success)
    {
        if(sim868::gsm::send_http_req(success,
            "http://api.open-meteo.com/v1/forecast?latitude=52.52&longitude=13.41&hourly=temperature_2m",
            forecast_json,
            4000)
            )
        {
            if(success == false)
            {
                printf("HTTP error\n");
                sleep_ms(10*1000);
            }
        }
        sleep_ms(1000);
    }

    std::cout << "forecast: " << forecast_json << std::endl;

    // sim868::sendRequestLong("AT+HTTPINIT", 2000);
    // sim868::sendRequestLong("AT+HTTPPARA=\"CID\",1", 2000);
    // sim868::sendRequestLong("AT+HTTPPARA=\"URL\","
    //     "\"http://api.open-meteo.com/v1/forecast?latitude=52.52&longitude=13.41&hourly=temperature_2m\"", 2000);
    
    // // sim868::sendRequestLong("AT+HTTPPARA=\"URL\","
    // //     "\"https://api.open-meteo.com/v1/forecast?latitude=52.52&longitude=13.41&hourly=temperature_2m,pressure_msl,precipitation,windspeed_10m,winddirection_10m,windgusts_10m&daily=sunrise,sunset,winddirection_10m_dominant&timezone=Europe%2FBerlin&start_date=2022-08-15&end_date=2022-08-15\""
    // //     , 2000);
    // // https://api.open-meteo.com/v1/forecast?latitude=52.52&longitude=13.41&hourly=temperature_2m,pressure_msl,precipitation,windspeed_10m,winddirection_10m,windgusts_10m&daily=sunrise,sunset,winddirection_10m_dominant&timezone=Europe%2FBerlin&start_date=2022-08-15&end_date=2022-08-15
    // if(sim868::is_respond_ok(sim868::sendRequestLong("AT+HTTPACTION=0", 5000)))
    // {
    //     sleep_ms(5000);
    //     auto resp = sim868::sendRequestLong("AT+HTTPREAD", 16000, 40);
    // }
    // sleep_ms(3000);
    // sim868::sendRequestLong("AT+HTTPTERM", 2000);


    // AT+CLBSCFG=0,1
    // AT+CLBSCFG=0,2
    // AT+CLBSCFG=0,3

    
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
        //consolef("\nSending to GSM:\"%s\"\n", buffer);
        //sendCMD_waitResp(buffer, "OK", 8000);
        //printf("mutex enter\n");
        uart_puts(UART_ID, buffer);
        uart_puts(UART_ID, "\r\n");
        mutex_exit(&uart_mutex);
        //sendCMD_waitResp(buffer,"",2000);
    }
    sim868::turnOff();
    //powerDown;
    //at_test();
}


int main(void)
{
    stdio_init_all();
    while (!stdio_usb_connected())
    {
        sleep_ms(100);
    }
    
    sleep_ms(1000);
    consoleLogInit();
    sleep_ms(2000);
    tracesSetup();
    TRACES_ON(1, TRACE_SIM868);

    // wait for intput
    // while (true)
    // {
    //     printf("waiting for input...\n");
    //     char c = getchar_timeout_us(1000000);
    //     if ((c == '\r') || (c == '\n'))
    //     {
    //         break;
    //     }
    // }

    consoleGSM();
}