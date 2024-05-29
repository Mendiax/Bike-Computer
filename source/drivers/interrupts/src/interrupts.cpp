#include <cstddef>
#include <math.h>
#include <stdio.h>
#include <string.h>

#include <pico/time.h>

#include <interrupts/interrupts.hpp>
#include "buttons/buttons.h"

// include interrupts
extern Interrupt interrupt_speed;
extern Interrupt interrupt_cadence;
// extern Interrupt interrupt_mpu;

#define MAX_INTERRUPTS 3

static Interrupt interrupts_core_0[MAX_INTERRUPTS];
static Interrupt interrupts_core_1[MAX_INTERRUPTS];
static size_t interrupts_core_0_counter = 0;
static size_t interrupts_core_1_counter = 0;



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
static inline void check_interrupt(const Interrupt& inter, const uint gpio,const uint32_t events);
static inline void check_interrupt_btn(Button_Interface& btn, const uint gpio,const uint32_t events);

void interrupt_add(Interrupt interrupt, INTERRUPT_CORE core, bool pullup)
{
    switch (core) {
    case NONE:
        return;
    case CORE_0:
        setup_interrupt(interrupt, pullup, interrutpCallback_core0);
        interrupts_core_0[interrupts_core_0_counter++] = interrupt;
        break;
    case CORE_1:
        setup_interrupt(interrupt, pullup, interrutpCallback_core1);
        interrupts_core_0[interrupts_core_1_counter++] = interrupt;
      break;
    }
}

void interruptSetupCore0(void)
{
    setup_interrupt(interrupt_speed,   true,  interrutpCallback_core0);
    setup_interrupt(interrupt_cadence, true,  interrutpCallback_core0);
    // setup_interrupt(interrupt_mpu,     false, interrutpCallback_core0);
}

static void interrutpCallback_core0(uint gpio, uint32_t events)
{
    check_interrupt(interrupt_speed,   gpio, events);
    check_interrupt(interrupt_cadence, gpio, events);
    // check_interrupt(interrupt_mpu,     gpio, events);
    for(size_t i=0; i < interrupts_core_0_counter; i++) {
        check_interrupt(interrupts_core_0[i], gpio, events);
    }
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


static inline void check_interrupt(const Interrupt& inter, const uint gpio,const uint32_t events)
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