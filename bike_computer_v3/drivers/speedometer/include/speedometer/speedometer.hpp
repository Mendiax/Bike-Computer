#ifndef __SPEEDMETER_H__
#define __SPEEDMETER_H__

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "common_types.h"
// 22
#define PIN_SPEED 16

//defined wheel size [m]
//hand measure r = 34.8cm -> d = 0.696m
//from 27.5" -> d = 0.6985m
#define WHEEL_SIZE (0.696 * 3.1415)
// [m/s]
#define MIN_SPEED 2.0

float speed_mps_to_kmph(float speed_mps);

float speed_getDistance();

/*returns last read speed [m/s]*/
float speed_getSpeed();

void speed_new();

void speed_delete();

void speed_emulate(float speed = 20.0);

// void speedDataInit(SpeedData& speedData);

// void speedDataUpdate(SpeedData& speedData, SystemState& state);

namespace speed
{
    float get_velocity_kph();
    float get_distance_m();
    float kph_to_rpm(float kph);
    void start();
    void stop();
    void reset();
}


#endif
