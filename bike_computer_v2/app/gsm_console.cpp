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
        if(uart_is_readable_within_us(UART_ID0, 2000))
        {
            buffer[i] = uart_getc(UART_ID0);
            if (buffer[i] == '\n')
            {
                consolef("%s", buffer);
                memset(buffer, 0, sizeof(buffer));
            }
            i++;
        }
        mutex_exit(&uart_mutex);
    }
}

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

    //multicore_launch_core1(core1_entry);

    char buffer[256] = {0};
    while (strcmp(buffer, "exit"))
    {
        int len = scanf("%s", buffer);
        buffer[len + 1] = '\0';
        consolef("\nSending to GSM:\"%s\"\n", buffer);
        sendCMD_waitResp(buffer, "OK", 8000);
        // mutex_enter_blocking(&uart_mutex);
        // uart_puts(UART_ID0, buffer);
        // uart_puts(UART_ID0, "\r\n");
        // mutex_exit(&uart_mutex);
        // sendCMD_waitResp(buffer,"",2000);
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