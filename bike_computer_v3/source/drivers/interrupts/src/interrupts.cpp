#include <speedometer/speedometer.hpp>
#include "cadence/cadence.hpp"

#include <math.h>
#include <pico/time.h>
#include <hardware/gpio.h>
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

/**
 * @brief Set the up interrupt object
 *
 * @param inter
 */
static inline void setup_interrupt(const Interrupt& inter, bool pullup, void (*interrutp_cb)(uint, uint32_t));

/**
 * @brief Set the up interrupt btn object
 *
 * @param btn
 */
static inline void setup_interrupt_btn(Button_Interface& btn, void (*interrutp_cb)(uint, uint32_t));


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

static inline void setup_interrupt(const Interrupt& inter,bool pullup, void (*interrutp_cb)(uint, uint32_t))
{
    gpio_init(inter.pin);
    gpio_set_dir(inter.pin, GPIO_IN);
    if(pullup)
    {
        gpio_pull_up(inter.pin);
    }
    gpio_set_irq_enabled_with_callback(inter.pin, inter.event, true, interrutp_cb);
}

static inline void setup_interrupt_btn(Button_Interface& btn, void (*interrutp_cb)(uint, uint32_t))
{
    auto inter_pres = btn.get_interrupt_pressed();
    auto inter_rel = btn.get_interrupt_released();

    gpio_init(inter_pres.pin);
    gpio_set_dir(inter_pres.pin, GPIO_IN);
    gpio_pull_up(inter_pres.pin);
    gpio_set_irq_enabled_with_callback(inter_pres.pin, inter_pres.event, true, interrutp_cb);
    gpio_set_irq_enabled_with_callback(inter_pres.pin, inter_rel.event, true, interrutp_cb);
}

const Interrupt& Button_Interface::get_interrupt_pressed() const
{
    return this->interrupt_pressed;
}
const Interrupt& Button_Interface::get_interrupt_released() const
{
    return this->interrupt_released;
}