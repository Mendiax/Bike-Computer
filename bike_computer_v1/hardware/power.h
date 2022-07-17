#ifndef __POWER_H__
#define __POWER_H__

#include "pinout.h"

void powerUp()
{
    pinMode(PIN_POWER,OUTPUT);
    digitalWrite(PIN_POWER, LOW);
}

void powerDown()
{
    digitalWrite(PIN_POWER, HIGH);
}

unsigned long lastUpdate = 0;
unsigned long timeOut = 0;

// timeout in ms
void setTimeOut(const unsigned long time)
{
    timeOut = time;
}

void updateTimer()
{
    lastUpdate = millis();
}

bool checkTimeOut()
{
    return (millis() > timeOut + lastUpdate);
}

#endif