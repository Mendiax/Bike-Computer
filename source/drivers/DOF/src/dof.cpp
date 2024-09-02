
// #------------------------------#
// |          includes            |
// #------------------------------#
// c/c++ includes

// my includes
#include "dof.hpp"
#include "MPU9250.hpp"
#include "bmp280.hpp"
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

static void interrupt_dof_callback();

// #------------------------------#
// | global function definitions  |
// #------------------------------#

void dof::init(bool interrupt)
{
    bmp280::init();
    mpu9250::init();

    if(interrupt)
    {
        Interrupt interrupt_dof = {20, interrupt_dof_callback, GPIO_IRQ_EDGE_RISE};
        interrupt_add(interrupt_dof, CORE_0, false);
    }
}
void dof::update()
{
    bmp280::update();
    mpu9250::update();
}

// #------------------------------#
// | static functions definitions |
// #------------------------------#

static void interrupt_dof_callback()
{
    dof::update();
}