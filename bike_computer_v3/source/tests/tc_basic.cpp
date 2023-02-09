#include "traces.h"
#include <stdio.h>
#include <pico/stdlib.h>
#include <iostream>
#include <string>
#include <pico/bootrom.h>
// #include <pico/stdio_usb.h>

// #include <fmt/ranges.h>


int main()
{
    asm volatile("nop");
    gpio_init(25); //power led
    gpio_set_dir(25, GPIO_OUT);
    gpio_put(25, 0);
    // Initialize chosen serial port
    sleep_ms(1000);
    stdio_init_all();
    //time_init();
    while (!stdio_usb_connected())
    {
        sleep_ms(1000);
        gpio_put(25, 0);
        sleep_ms(1000);
        gpio_put(25, 1);

    }
    gpio_put(25, 1);
    tracesSetup();

    std::string cmd;
    std::cin >> cmd;
    PRINT(cmd);

    if(cmd == "boot")
    {
        reset_usb_boot(0, 0);
    }

    sleep_ms(1000);
    while (1) {
        printf("Hello world!\n");
        sleep_ms(1000);
    }

    return 0;
}