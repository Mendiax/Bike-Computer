#ifndef __SPEEDMETER_H__
#define __SPEEDMETER_H__

#include "../addons/ringbuffer.h"

static volatile unsigned long speed_lastupdate = 0;
//[m/s]
static volatile double speed_velocity = 0.0f;

//defined wheel size [m]
static double speed_wheelesize = 0.6985 * PI;

//check if last value was read by speed_getSpeed() function
static volatile bool read = true;

#define MIN_SPEED 2 // [m/s]

//time after speed is set to 0 [s]
static const double max_time = (speed_wheelesize / MIN_SPEED) * 1000.0;

static byte speed_pinread = -1;

double speed_mps_to_kmph(double speed_mps){
    return speed_mps * 3.6;
}

/*returns last read speed [m/s]*/
double speed_getSpeed()
{
    double update = millis();
    double speed = speed_velocity;
    if (update - speed_lastupdate > max_time)
    {
        speed = 0.0;
    }
    read = true;
    //Serial.println("V " + String(speed));
    //TRACE_SPEED_PRINT("getSpeed : " + String(update) + " speed : " + String(speed));
    return speed;
}

static void speed_update()
{

    unsigned long update = millis();
    unsigned long delta_time = update - speed_lastupdate;
    //Serial.println("dt " + String(delta_time));
    if (delta_time < 79)
    {
        return;
    }

    speed_velocity = speed_wheelesize / ((float)delta_time / 1000.0);

    Serial.print("Interrupt time : ");
    Serial.print(update);
    Serial.print(" speed : ");
    Serial.print(speed_velocity);
    Serial.print(" delta time : ");
    Serial.println(delta_time);
    //TRACE_SPEED_PRINT("Interrupt time : " + String(update) + " speed : " + String(speed_velocity) + " delta time : " + String(delta_time));
    
    if (read)
    {
        speed_lastupdate = update;
        read = false;
    }
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
