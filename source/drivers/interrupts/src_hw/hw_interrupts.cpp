
// #------------------------------#
// |          includes            |
// #------------------------------#
// pico includes
#include <hardware/gpio.h>

// c/c++ includes

// my includes
#include <interrupts/interrupts.hpp>

// #------------------------------#
// |           macros             |
// #------------------------------#

// #------------------------------#
// | global variables definitions |
// #------------------------------#

// #------------------------------#
// | static variables definitions |
// #------------------------------#

// #------------------------------#
// | static functions declarations|
// #------------------------------#

// #------------------------------#
// | global function definitions  |
// #------------------------------#
void setup_interrupt(const Interrupt& inter,bool pullup, void (*interrutp_cb)(uint, uint32_t))
{
    gpio_init(inter.pin);
    gpio_set_dir(inter.pin, GPIO_IN);
    if(pullup)
    {
        gpio_pull_up(inter.pin);
    }
    gpio_set_irq_enabled_with_callback(inter.pin, inter.event, true, interrutp_cb);
}

void setup_interrupt_btn(Button_Interface& btn, void (*interrutp_cb)(uint, uint32_t))
{
    auto inter_pres = btn.get_interrupt_pressed();
    auto inter_rel = btn.get_interrupt_released();

    gpio_init(inter_pres.pin);
    gpio_set_dir(inter_pres.pin, GPIO_IN);
    gpio_pull_up(inter_pres.pin);
    gpio_set_irq_enabled_with_callback(inter_pres.pin, inter_pres.event, true, interrutp_cb);
    gpio_set_irq_enabled_with_callback(inter_pres.pin, inter_rel.event, true, interrutp_cb);
}

// #------------------------------#
// | static functions definitions |
// #------------------------------#
