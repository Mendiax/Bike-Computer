#include <speedometer/speedometer.h>

#include <math.h>
#include <pico/time.h>
#include <hardware/gpio.h>
#include <stdio.h>
#include <string.h>
#include <interrupts/interrupts.hpp>


// include interrupts
extern interrupt interruptSpeed;

static void interrutpCallback(uint gpio, uint32_t events);


void interruptSetup(void)
{
    // SETUP ALL INTERRUPT PINS
    
    // speed interrupt
    gpio_init(interruptSpeed.pin);
    gpio_set_dir(interruptSpeed.pin, GPIO_IN);
    gpio_pull_up(interruptSpeed.pin);
    gpio_set_irq_enabled_with_callback(interruptSpeed.pin, interruptSpeed.event, true, interrutpCallback);
}

// TODO second core

static inline void checkInterrupt(interrupt inter, uint gpio, uint32_t events)
{                                                      
    if (inter.pin == gpio && events & inter.event) 
    {                                                  
        inter.callbackFunc();                          
    }                                                  
}

static void interrutpCallback(uint gpio, uint32_t events)
{
    checkInterrupt(interruptSpeed, gpio, events);
}
