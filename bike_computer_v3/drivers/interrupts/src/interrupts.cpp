#include <speedometer/speedometer.hpp>

#include <math.h>
#include <pico/time.h>
#include <hardware/gpio.h>
#include <stdio.h>
#include <string.h>
#include <interrupts/interrupts.hpp>
#include "buttons/buttons.h"

// include interrupts
extern interrupt interruptSpeed;

// buttons
// extern interrupt button0;
// extern interrupt button1;
// extern interrupt button1rel;
// extern interrupt button2;
// extern interrupt button2rel;

// extern interrupt button3;



static void interrutpCallback_core0(uint gpio, uint32_t events);
static void interrutpCallback_core1(uint gpio, uint32_t events);

static inline void checkInterrupt(const interrupt& inter, const uint gpio,const uint32_t events);
static inline void check_interrupt_btn(Button_Interface& btn, const uint gpio,const uint32_t events);
static inline void setup_interrupt_btn(Button_Interface& btn);



void interruptSetupCore0(void)
{
    // SETUP ALL INTERRUPT PINS

    // speed interrupt
    gpio_init(interruptSpeed.pin);
    gpio_set_dir(interruptSpeed.pin, GPIO_IN);
    //gpio_pull_up(interruptSpeed.pin);
    gpio_set_irq_enabled_with_callback(interruptSpeed.pin, interruptSpeed.event, true, interrutpCallback_core0);
}

static void interrutpCallback_core0(uint gpio, uint32_t events)
{
    checkInterrupt(interruptSpeed, gpio, events);
}

/**
 * @brief must be called from core1 thread
 *
 */
void interruptSetupCore1(void)
{
    setup_interrupt_btn(btn0);
    setup_interrupt_btn(btn1);
    setup_interrupt_btn(btn2);
    setup_interrupt_btn(btn3);

}
static void interrutpCallback_core1(uint gpio, uint32_t events)
{
    check_interrupt_btn(btn0, gpio, events);
    check_interrupt_btn(btn1, gpio, events);
    check_interrupt_btn(btn2, gpio, events);
    check_interrupt_btn(btn3, gpio, events);
}


static inline void checkInterrupt(const interrupt& inter, const uint gpio,const uint32_t events)
{
    if (inter.pin == gpio && (events & inter.event))
    {
        inter.callbackFunc();
    }
}

static inline void check_interrupt_btn(Button_Interface& btn, const uint gpio,const uint32_t events)
{
    {
        auto inter = btn.get_interrupt_pressed();
        if (inter.pin == gpio && (events & inter.event))
        {
            btn.on_call_press();
        }
    }
    {
        auto inter = btn.get_interrupt_released();
        if (inter.pin == gpio && (events & inter.event))
        {
            btn.on_call_release();
        }
    }
}

static inline void setup_interrupt_btn(Button_Interface& btn)
{
    auto inter_pres = btn.get_interrupt_pressed();
    auto inter_rel = btn.get_interrupt_released();

    gpio_init(inter_pres.pin);
    gpio_set_dir(inter_pres.pin, GPIO_IN);
    gpio_pull_up(inter_pres.pin);
    gpio_set_irq_enabled_with_callback(inter_pres.pin, inter_pres.event, true, interrutpCallback_core1);
    gpio_set_irq_enabled_with_callback(inter_pres.pin, inter_rel.event, true, interrutpCallback_core1);
}

const interrupt& Button_Interface::get_interrupt_pressed() const
{
    return this->interrupt_pressed;
}
const interrupt& Button_Interface::get_interrupt_released() const
{
    return this->interrupt_released;
}