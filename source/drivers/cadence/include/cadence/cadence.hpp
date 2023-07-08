#ifndef __CADENCE_H__
#define __CADENCE_H__

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "common_types.h"

#define PIN_CADENCE 22

// [m/s]
#define MIN_CADENCE 10


namespace cadence
{
    float get_cadence();
    void emulate(float cadence=80.0);
};


#endif
