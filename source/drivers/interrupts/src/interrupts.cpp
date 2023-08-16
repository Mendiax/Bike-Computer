// #include <speedometer/speedometer.hpp>
// #include "cadence/cadence.hpp"

#include <math.h>
#include <pico/time.h>
#include <stdio.h>
#include <string.h>
#include <interrupts/interrupts.hpp>
#include "buttons/buttons.h"

// include interrupts
extern Interrupt interruptSpeed;
extern Interrupt interrupt_cadence;

// buttons


/**
 * @brief function that is called on interrupt for core 0
 *
 * @param gpio
 * @param events
 */
static void interrutpCallback_core0(uint gpio, uint32_t events);
/**
 * @brief function that is called on interrupt for core 1
 *
 * @param gpio
 * @param events
 */
static void interrutpCallback_core1(uint gpio, uint32_t events);


// Those functions are implemented in hw dependent files
// see src_hw and src_mock
/**
 * @brief Set the up interrupt object
 *
 * @param inter
 */
void setup_interrupt(const Interrupt& inter, bool pullup, void (*interrutp_cb)(uint, uint32_t));

/**
 * @brief Set the up interrupt btn object
 *
 * @param btn
 */
void setup_interrupt_btn(Button_Interface& btn, void (*interrutp_cb)(uint, uint32_t));


/**
 * @brief checks if inter function should be triggered
 *
 * @param inter
 * @param gpio
 * @param events
 */
static inline void checkInterrupt(const Interrupt& inter, const uint gpio,const uint32_t events);
static inline void check_interrupt_btn(Button_Interface& btn, const uint gpio,const uint32_t events);


void interruptSetupCore0(void)
{
    setup_interrupt(interruptSpeed, true, interrutpCallback_core0);
    setup_interrupt(interrupt_cadence, true, interrutpCallback_core0);
}

static void interrutpCallback_core0(uint gpio, uint32_t events)
{
    checkInterrupt(interruptSpeed, gpio, events);
    checkInterrupt(interrupt_cadence, gpio, events);
}

/**
 * @brief must be called from core1 thread
 *
 */
void interruptSetupCore1(void)
{
    setup_interrupt_btn(btn0, interrutpCallback_core1);
    setup_interrupt_btn(btn1, interrutpCallback_core1);
    setup_interrupt_btn(btn2, interrutpCallback_core1);
    setup_interrupt_btn(btn3, interrutpCallback_core1);

}
static void interrutpCallback_core1(uint gpio, uint32_t events)
{
    check_interrupt_btn(btn0, gpio, events);
    check_interrupt_btn(btn1, gpio, events);
    check_interrupt_btn(btn2, gpio, events);
    check_interrupt_btn(btn3, gpio, events);
}


static inline void checkInterrupt(const Interrupt& inter, const uint gpio,const uint32_t events)
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



const Interrupt& Button_Interface::get_interrupt_pressed() const
{
    return this->interrupt_pressed;
}
const Interrupt& Button_Interface::get_interrupt_released() const
{
    return this->interrupt_released;
}