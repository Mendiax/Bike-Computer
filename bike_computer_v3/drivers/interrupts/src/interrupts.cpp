#include <speedometer/speedometer.hpp>

#include <math.h>
#include <pico/time.h>
#include <hardware/gpio.h>
#include <stdio.h>
#include <string.h>
#include <interrupts/interrupts.hpp>


// include interrupts
extern interrupt interruptSpeed;

// buttons
extern interrupt button0;
extern interrupt button1;
extern interrupt button2;
extern interrupt button3;

static void interrutpCallback(uint gpio, uint32_t events);


void interruptSetupCore0(void)
{
    // SETUP ALL INTERRUPT PINS
    
    // speed interrupt
    gpio_init(interruptSpeed.pin);
    gpio_set_dir(interruptSpeed.pin, GPIO_IN);
    gpio_pull_up(interruptSpeed.pin);
    gpio_set_irq_enabled_with_callback(interruptSpeed.pin, interruptSpeed.event, true, interrutpCallback);
}

/**
 * @brief must be called from core1 thread
 * 
 */
void interruptSetupCore1(void)
{
    gpio_init(button0.pin);
    gpio_set_dir(button0.pin, GPIO_IN);
    gpio_pull_up(button0.pin);
    gpio_set_irq_enabled_with_callback(button0.pin, button0.event, true, interrutpCallback);
    gpio_init(button1.pin);
    gpio_set_dir(button1.pin, GPIO_IN);
    gpio_pull_up(button1.pin);
    gpio_set_irq_enabled_with_callback(button1.pin, button1.event, true, interrutpCallback);
    // for testing
    // gpio_init(button2.pin);
    // gpio_set_dir(button2.pin, GPIO_IN);
    // gpio_pull_up(button2.pin);
    // gpio_set_irq_enabled_with_callback(button2.pin, button2.event, true, interrutpCallback);
    gpio_init(button3.pin);
    gpio_set_dir(button3.pin, GPIO_IN);
    gpio_pull_up(button3.pin);
    gpio_set_irq_enabled_with_callback(button3.pin, button3.event, true, interrutpCallback);

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

    checkInterrupt(button0, gpio, events);
    checkInterrupt(button1, gpio, events);
    checkInterrupt(button2, gpio, events);
    checkInterrupt(button3, gpio, events);

}
