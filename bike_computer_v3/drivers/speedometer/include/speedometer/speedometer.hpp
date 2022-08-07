#ifndef __SPEEDMETER_H__
#define __SPEEDMETER_H__

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "types.h"

#define PIN_SPEED 22

//defined wheel size [m]
//hand measure r = 34.8cm -> d = 0.696m
//from 27.5" -> d = 0.6985m
#define WHEEL_SIZE (0.696 * 3.1415)
// [m/s]
#define MIN_SPEED 0.5 

float speed_mps_to_kmph(float speed_mps);

unsigned speed_getDistance();

/*returns last read speed [m/s]*/
float speed_getSpeed();

void speed_new();

void speed_delete();

void speed_emulate(int16_t speed = 20);

void speedDataInit(SpeedData& speedData);

void speedDataUpdate(SpeedData& speedData);


#endif
