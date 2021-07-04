#ifndef __SPEEDMETER_H__
#define __SPEEDMETER_H__

#include "ringbuffer.h"

unsigned long speed_lastupdate = 0;
float speed_velocity = 0.0f; 
float  speed_wheelesize;
byte speed_pinread = -1;
RingBuffer* speed_buffer;

void speed_update(){
    float update = millis();
    unsigned long delta_time = update - speed_lastupdate;
    speed_velocity = speed_wheelesize * (float)delta_time;
    speed_lastupdate = update;
    ring_buffer_push_overwrite(speed_buffer,(char*)&speed_velocity);
}

void speed_setup(byte pin, size_t bufferlength){
    speed_buffer = ring_buffer_create(sizeof(float), bufferlength);
    speed_pinread = pin;
    pinMode(speed_pinread, INPUT);
    attachInterrupt(digitalPinToInterrupt(speed_pinread), speed_lastupdate, RISING);
}

void speed_delete(){
    ring_buffer_destroy(speed_buffer);
    detachInterrupt(digitalPinToInterrupt(speed_pinread));
}

#endif