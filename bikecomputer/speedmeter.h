#ifndef __SPEEDMETER_H__
#define __SPEEDMETER_H__

#include "ringbuffer.h"

unsigned long speed_lastupdate = 0;
float speed_velocity = 0.0f;
float speed_wheelesize = 2 * 0.6985 * PI ;
bool read = true;

float min_speed = 2;

static float max_time = (speed_wheelesize / min_speed) * 1000.0;

byte speed_pinread = -1;
RingBuffer *speed_buffer;

/*waits for speed update with timeout*/
float speed_get_speed()
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

void speed_update()
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

void speed_setup(byte pin, size_t bufferlength)
{
    speed_buffer = ring_buffer_create(sizeof(float), bufferlength);
    speed_pinread = pin;
    pinMode(speed_pinread, INPUT);
    attachInterrupt(digitalPinToInterrupt(speed_pinread), speed_update, RISING);
}

void speed_delete()
{
    ring_buffer_destroy(speed_buffer);
    detachInterrupt(digitalPinToInterrupt(speed_pinread));
}

#endif
