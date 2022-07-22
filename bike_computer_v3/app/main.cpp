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

#include "speedometer/speedometer.h"
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

void updateSpeedData(SensorData& sensorData)
{
    // get data for speed and distance
    static float velocity;
    speed_emulate();
// #if SIM_INPUT
//     velocity += 1;
//     velocity = velocity > 30.0 ? 0.0 : velocity;
//     sensorData.speedDistance += 10;
//     sensorData.speedDistanceHundreth += velocity;
//    sensorData.speedDistanceHundreth = sensorData.speedDistanceHundreth % 100;
//#else
    velocity = speed_mps_to_kmph(speed_getSpeed());
    sensorData.speed.speedDistance = speed_getDistance() / 1000;
    sensorData.speed.speedDistanceHundreth = (speed_getDistance() / 100) % 100;
//#endif

    // do some calculations
    sensorData.speed.speedMax = std::max(sensorData.speed.speedMax, velocity);
    sensorData.speed.speedAvgVal = sensorData.speed.speedDistance / ((double)sensorData.time / 3600.0);


    // update buffer
    uint8_t bufferVelocity = (uint8_t)velocity;
    ring_buffer_push_overwrite(sensorData.speed.speedBuffer, (char *)&bufferVelocity);
}

//SensorData sensorData = {0};
/*
    setup function
*/
int main()
{
    //traces setup
    tracesSetup();

    stdio_init_all();
    while (!stdio_usb_connected()){sleep_ms(100);}
    TRACE_DEBUG(0, TRACE_MAIN, "Main start\n");

    TRACE_DEBUG(0, TRACE_MAIN, "interrupt setup core 0\n");
    interruptSetupCore0();



    //turn of power led
    gpio_init(25); //power led
    gpio_set_dir(25, GPIO_OUT);
    gpio_put(25, 1);

    // ui btn setup
    // gpio_init(BTN);
    // gpio_set_dir(BTN, GPIO_IN);
    // gpio_pull_up(BTN);

    // battery setup
    adc_init();
    //adc_gpio_init(26); //temp
    batterySetup();


    sensorData.rearShockBuffer = ring_buffer_create(sizeof(uint8_t), SCREEN_WIDTH);
    sensorData.speed.speedBuffer = ring_buffer_create(sizeof(uint8_t), SCREEN_WIDTH);
    TRACE_DEBUG(0, TRACE_MAIN, "Launch core1\n");

    multicore_launch_core1(core1LaunchThread);
    while(1)
    {
        loop();
    }
}

void loop()
{
    TRACE_DEBUG(0, TRACE_MAIN, "Main loop() start\n");
    mutex_enter_blocking(&sensorDataMutex);

    sensorData.lipoLevel = getBatteryLevel();

    updateSpeedData(sensorData);

    absolute_time_t currentTime = get_absolute_time();
    sensorData.time = to_ms_since_boot(currentTime) / 1000;

    mutex_exit(&sensorDataMutex);

    return;

//     // lower refresh rate to save some energy
//     static absolute_time_t lastDisplayUpdate;
//     int64_t delta_time_ms = us_to_ms(absolute_time_diff_us(lastDisplayUpdate, currentTime));
//     if (delta_time_ms < 100)
//     {
//         // TODO
//         // add sleep time to save energy after creating interrupt for btn
//         // int64_t timeLeft = 100 - delta_time_ms;
//         return;
//     }
// #if PRINT_FRAME_TIME
//     absolute_time_t updateStart = get_absolute_time();
//     Display_update();
//     absolute_time_t updateEnd = get_absolute_time();
//     int64_t updateTime = us_to_ms(absolute_time_diff_us(updateStart, updateEnd));
//     std::string log = "update:" + std::to_string(updateTime) + "ms";

//     Paint_Println( 0, 0, log.c_str(), &Font8, 0x0f, 0x00);
//     display::display();
//     sleep_ms(100);
// #else
//     Display_update();
//     getBatteryLevel();
//     getTemp();
    
//     //std::string log = "btn:" + std::to_string(gpio_get(BTN));
//     //Paint_Println(0 * Font8.width, 1 * Font8.height, log.c_str(), &Font8, 0x0f, 0x00);
//     display::display();
// #endif
//     //Screen_draw();// TODO fix battery
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

