#ifndef INTERRUPT_H
#define INTERRUPT_H

#include "pico/stdlib.h"
#include "hardware/gpio.h"

typedef void (*interr)(void);

struct interrupt
{
    int pin;
    interr callbackFunc;
    gpio_irq_level event;
};

void interruptSetup(void);

#endif
