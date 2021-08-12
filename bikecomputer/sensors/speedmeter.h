#ifndef __SPEEDMETER_H__
#define __SPEEDMETER_H__

#include "ringbuffer.h"

//updated in speed_update()
static volatile unsigned long speed_lastupdate = 0;
static volatile float speed_velocity = 0.0f;

//defined wheel size 
static float speed_wheelesize;

//check if last value was read by speed_getSpeed() function
static bool read = true;

#define MIN_SPEED 2 //in m/s

//time after speed is set to 0
static const float max_time = (speed_wheelesize / MIN_SPEED) * 1000.0;

static byte speed_pinread = -1;

/*waits for speed update with timeout*/
float speed_getSpeed()
{
    float update = millis();
    float speed = speed_velocity;
    if (update - speed_lastupdate > max_time)
    {
        speed = 0.0;
    }
    read = true;
    return speed;
}

static void speed_update()
{

    float update = millis();
    unsigned long delta_time = update - speed_lastupdate;
    if (delta_time < 78.996159)
    {
        return;
    }
    float prev_v = speed_velocity;
    float curr_v = speed_wheelesize / ((float)delta_time / 1000.0);
    //if (abs(prev_v - curr_v) > 5.0 && delta_time < 1000.0)
    speed_velocity = curr_v;
    if (read)
    {
        speed_lastupdate = update;
        read = false;
    }

    /*#ifdef PCDEBUG
    Serial.println(speed_velocity);
#endif*/
}

void speed_new(byte pin, double wheelSize)
{
    speed_wheelesize = wheelSize;
    speed_pinread = pin;
    pinMode(speed_pinread, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(speed_pinread), speed_update, FALLING );
}

void speed_delete()
{
    detachInterrupt(digitalPinToInterrupt(speed_pinread));
}

#endif
