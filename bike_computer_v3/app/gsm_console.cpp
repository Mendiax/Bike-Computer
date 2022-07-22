#include <stdio.h>
#include <string.h>
//#include <stdarg.h>
#include "pico/stdio_usb.h"


#include <pico/stdlib.h>
#include <pico/multicore.h>
#include <pico/sync.h>
#include "hardware/uart.h"

#define ROTATE
//#include "display/print.h"
#include "console/console.h"
#include "Pico-SIM868-Test.h"

auto_init_mutex(uart_mutex);

void core1_entry()
{
    size_t i = 0;
    char buffer[500] = {0};
    while (1)
    {   
        mutex_enter_blocking(&uart_mutex);
        while (uart_is_readable_within_us(UART_ID0, 100 * 1000)) // 100ms
        {
            buffer[i] = uart_getc(UART_ID0);
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
    DEV_GSM_Module_Init();
    powerOn;
    led_blink();
    DEV_GSM_Delay_ms(5000);

    check_start();

    // sendCMD_waitResp("AT+BTPOWER=1", "OK", 3000);
    // sendCMD_waitResp("AT+BTHOST?", "OK", 3000);
    // sendCMD_waitResp("AT+BTSTATUS?", "OK", 3000);
    // sendCMD_waitResp("AT+BTSCAN=1,10", "OK", 8000);
    





    // GSM setup
    // sendCMD_waitResp("AT+CSQ","OK",1000);
    // sendCMD_waitResp("AT+CSTT=\"internet\",\"internet\",\"internet\"", "OK", 8000);
    // //should be CGDCONT: 1,"IP","internet","0.0.0.0",0,0
    // sendCMD_waitResp("AT+CIICR", "OK", 1000);
    // sendCMD_waitResp("AT+CIFSR", "OK", 1000);
    // sendCMD_waitResp("AT+CIPPING=\"www.google.com\"", "OK", 8000);

    // // bearer config
    // sendCMD_waitResp("AT+SAPBR=3,1,\"APN\",\"internet\"","OK", 2000);
    // sendCMD_waitResp("AT+SAPBR=3,1,\"USER\",\"internet\"","OK", 2000);
    // sendCMD_waitResp("AT+SAPBR=3,1,\"PWD\",\"internet\"","OK", 2000);
    // sendCMD_waitResp("AT+SAPBR=1,1", "OK",2000);
    // sendCMD_waitResp("AT+SAPBR=2,1", "OK",2000);



    // sendCMD_waitResp("AT+HTTPINIT", "OK", 2000);
    // sendCMD_waitResp("AT+HTTPPARA=\"CID\",1", "OK", 2000);
    // sendCMD_waitResp("AT+HTTPPARA=\"URL\",\"http://api.open-meteo.com/v1/forecast?latitude=52.52&longitude=13.41&hourly=temperature_2m\"", "OK", 2000);
    // if(sendCMD_waitResp("AT+HTTPACTION=0", "200", 5000))
    // {
    //     sendCMD_waitResp("AT+HTTPREAD", "OK", 3000);
    // }
    // sendCMD_waitResp("AT+HTTPTERM", "OK", 2000);


    // AT+CLBSCFG=0,1
    // AT+CLBSCFG=0,2
    // AT+CLBSCFG=0,3








 int count = 0;
    sendCMD_waitResp("AT+CGNSPWR=1", "OK", 2000);
    DEV_GSM_Delay_ms(2000);
    for (int i = 1; i < 10; i++)
    {
        if (sendCMD_waitResp("AT+CGNSINF", ",,,,", 2000) == 1)
        {
            consolep("GPS is not ready\r\n");
            if (i >= 9)
            {
                consolep("GPS positioning failed, please check the GPS antenna!\r\n");
                sendCMD_waitResp("AT+CGNSPWR=0", "OK", 2000);
            }
            else
            {
                consolep("wait...\r\n");
                DEV_GSM_Delay_ms(2000);
                continue;
            }
        }
        else
        {
            if (count <= 3)
            {
                count++;
                consolep("GPS info:\r\n");
            }
            else
            {
                sendCMD_waitResp("AT+CGNSPWR=0", "OK", 2000);
                break;
            }
        }
    }

    
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
        uart_puts(UART_ID0, buffer);
        uart_puts(UART_ID0, "\r\n");
        mutex_exit(&uart_mutex);
        //sendCMD_waitResp(buffer,"",2000);
    }
    powerDown;
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
    sleep_ms(1000);

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