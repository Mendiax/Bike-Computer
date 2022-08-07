#include <stdlib.h>
#include <cstdint>
#include <stdio.h>
#include <iostream>
#include <cmath>

#include <pico/stdlib.h>
#include <hardware/adc.h>

// display
#include "display/print.h"
#include "display/debug.h"

//veiws
#include "views/display.h"

#include "speedometer/speedometer.hpp"
#include <interrupts/interrupts.hpp>

#include <pico/multicore.h>
#include <pico/sync.h>

#include <traces.h>


#include <types.h>
#include <core1.h>

#include "battery/battery.h"


//static bool isBtnPressed(uint gpio);
static float getTemp();

#define PRINT_FRAME_TIME 0
#define PRINT_LIPO_DATA 1


static void loop();



//SensorData sensorData = {0};
/*
    setup function
*/
int main()
{
    //traces setup
    tracesSetup();

    // setup console
    stdio_init_all();
    // wait for serial console
    // coment out for normal use
    //while (!stdio_usb_connected()){sleep_ms(100);}
    TRACE_DEBUG(0, TRACE_MAIN, "Main start\n");

    TRACE_DEBUG(0, TRACE_MAIN, "interrupt setup core 0\n");
    interruptSetupCore0();

    //turn of power led
    gpio_init(25); //power led
    gpio_set_dir(25, GPIO_OUT);
    gpio_put(25, 1);

    // battery setup
    adc_init();
    //adc_gpio_init(26); //temp
    batterySetup();

    speedDataInit(sensorData.speed);
    //speed_emulate();

    //sensorData.rearShockBuffer = ring_buffer_create(sizeof(uint8_t), SCREEN_WIDTH);
    TRACE_DEBUG(0, TRACE_MAIN, "Launch core1\n");

    multicore_launch_core1(core1LaunchThread);
    while(1)
    {
        loop();
    }
}

void loop()
{
    mutex_enter_blocking(&sensorDataMutex);

    speedDataUpdate(sensorData.speed);
    sensorData.lipoLevel = getBatteryLevel();

    sensorData.time = to_ms_since_boot(get_absolute_time()) / 1000;

    mutex_exit(&sensorDataMutex);

    return;
}

// DO NOT REMOVE
/*-----static definitions------*/
// static bool isBtnPressed(uint gpio)
// {
//     static absolute_time_t lastPress;
//     absolute_time_t currentTime = get_absolute_time();
//     int64_t delta_time_ms = us_to_ms(absolute_time_diff_us(lastPress, currentTime));
//     if (delta_time_ms < 100)
//     {
//         return false;
//     }
//     static bool released;
//     released = true; // TODO add this functionality
//     lastPress = get_absolute_time();
//     bool ret = released ? !gpio_get(BTN) /*negate bc it is pullup*/ : false;

//     if(ret)
//         TRACE_DEBUG(0, BUTTONS, "isBtnPressed return=%dms\n", ret);
//     return ret;
// }



// DO NOT REMOVE !!!
// static float getTemp()
// {
//     float conversion_factor = 3.28f / (1 << 12);
//     adc_select_input(0);
//     float voltage = adc_read() * conversion_factor;

//     const float U = 3.28f;
//     const float R1 = 3270;
//     const float R3 = 1;
//     float Rt = (voltage/(U-voltage)) * R1 - R3;

//     const float T0 = 298.15;
//     const float R0 = 9170;
//     const float B = 4000;
//     const float T = (B * T0)/\
//                     (T0*std::log(Rt/R0) + B);

//     const int Tc = T - 273.15;


//     std::string log = "temp:" + std::to_string(Tc) + "C";
//     Paint_Println(16 * Font8.width, 0, log.c_str(), &Font8);
//     //__display->write();
//     return voltage;
// }

