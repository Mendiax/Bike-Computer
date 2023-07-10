
// #------------------------------#
// |          includes            |
// #------------------------------#
// pico includes

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
    (void)inter;
    (void)pullup;
    (void)interrutp_cb;
}

void setup_interrupt_btn(Button_Interface& btn, void (*interrutp_cb)(uint, uint32_t))
{
    (void)btn;
    (void)interrutp_cb;
}

// #------------------------------#
// | static functions definitions |
// #------------------------------#
