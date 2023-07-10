#include "bmp280.hpp"
#include "bmp280_raw.hpp"
#include "filterlowpass.hpp"

#include <cstddef>
#include <stdint.h>
#include <stdio.h>
#include <math.h>

#include "pico/stdlib.h"
#include "traces.h"
#include "ringbuffer.h"



#define BUFFER_LEN 10
static Ring_Buffer* press_buffer = 0;


static Filter_Low_Pass<float>* filter = 0;


std::tuple<float, float> bmp280::get_temp_press()
{
    bmp280_update_data();
    float temperature = bmp280_get_temp();
    float pressure = bmp280_get_press();


    TRACE_PLOT(1, "pressure raw", pressure);

    ring_buffer_push_overwrite(press_buffer, (char*)&pressure);

    float* buffer_arr = (float*)press_buffer->data_pointer;
    float press_avg = 0;
    for(size_t i = 0; i < press_buffer->current_queue_length; i++)
    {
        press_avg += buffer_arr[i];
    }
    press_avg /= press_buffer->current_queue_length;
    TRACE_PLOT(1, "pressure avg filter", press_avg);

    float press_filter = filter->apply(pressure);
    TRACE_PLOT(1, "pressure low pass filter", press_filter);


    return std::make_tuple(temperature, press_filter);
}

void bmp280::init()
{
    press_buffer = ring_buffer_create(sizeof(float), BUFFER_LEN);
    filter = new Filter_Low_Pass<float>(0.1);

    bmp280_init();

    // get some init data
    for(int i = 0; i < 4; i++)
    {
        bmp280::get_temp_press();
        sleep_ms(10);
    }
}

float bmp280::get_height(float pressure_msl, float pressure_cl, float temp)
{
    float h = (
            (std::pow(((double)pressure_msl/(double)pressure_cl), (1.0/5.257)) - 1.0) *
             (temp + 273.15)
             ) / 0.0065;
    return h;
}

// int bmp_test() {
//     // configure BMP280
//     bmp280_init();

//     int32_t raw_temperature;
//     int32_t raw_pressure;

//     sleep_ms(250); // sleep so that data polling and register update don't collide
//     while (1) {
//         bmp280_read_raw(&raw_temperature, &raw_pressure);
//         int32_t temperature = bmp280_convert_temp(raw_temperature, &params);
//         int32_t pressure = bmp280_convert_pressure(raw_pressure, raw_temperature, &params);
//         printf("Pressure = %.3f kPa\n", pressure / 1000.f);
//         printf("Temp. = %.2f C\n", temperature / 100.f);
//         // poll every 500ms
//         sleep_ms(500);
//     }

//     return 0;
// }



