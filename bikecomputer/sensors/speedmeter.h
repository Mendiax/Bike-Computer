#ifndef __SPEEDMETER_H__
#define __SPEEDMETER_H__

#include "../addons/ringbuffer.h"
#define PIN_SPEED 2
//defined wheel size [m]
#define WHEEL_SIZE (0.6985 * PI)
// [m/s]
#define MIN_SPEED 0.5 
//time after speed is set to 0 [s]
#define MAX_TIME (WHEEL_SIZE / MIN_SPEED * 1000.0)

static volatile unsigned long speed_lastupdate = 0;
//[m/s]
static volatile float speed_velocity = 0.0f;

static volatile unsigned speed_wheelCounter = 0;

//check if last value was read by speed_getSpeed() function
static volatile bool read = true;

float speed_mps_to_kmph(float speed_mps)
{
    return speed_mps * 3.6;
}
unsigned speed_getDistance(){
    return floor((double)speed_wheelCounter * WHEEL_SIZE * 3.6);
}

/*returns last read speed [m/s]*/
float speed_getSpeed()
{
    unsigned long update = millis();
    if (update - speed_lastupdate > MAX_TIME)
    {
        read = true;
        return 0.0;
    }
    read = true;
    //TRACE_SPEED_PRINT("getSpeed : " + String(update) + " speed : " + String(speed));
    return speed_velocity;
}

static void speed_update()
{

    unsigned long update = millis();
    unsigned long delta_time = update - speed_lastupdate;
    if (delta_time < 79)
    {
        return;
    }
    speed_wheelCounter++;
    speed_velocity = WHEEL_SIZE / ((float)delta_time / 1000.0);
    //TRACE_SPEED_PRINT("Interrupt time : " + String(update) + " speed : " + String(speed_velocity) + " delta time : " + String(delta_time));

    if (read)
    {
        speed_lastupdate = update;
        read = false;
    }
}

void speed_new()
{
    pinMode(PIN_SPEED, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(PIN_SPEED), speed_update, FALLING);
}

void speed_delete()
{
    detachInterrupt(digitalPinToInterrupt(PIN_SPEED));
}

#endif
