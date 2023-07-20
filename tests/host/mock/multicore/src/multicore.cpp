
// #------------------------------#
// |          includes            |
// #------------------------------#
// pico includes

// c/c++ includes

// my includes
#include "multicore.hpp"
#include <thread>

// #------------------------------#
// |           macros             |
// #------------------------------#

// #------------------------------#
// | global variables definitions |
// #------------------------------#

// #------------------------------#
// | static variables definitions |
// #------------------------------#
static std::thread* core1;

// #------------------------------#
// | static functions declarations|
// #------------------------------#

// #------------------------------#
// | global function definitions  |
// #------------------------------#
extern "C"
{
    void multicore_launch_core1(void (*entry)(void))
    {
        if (core1)
        {
            delete core1;
        }
        core1 = new std::thread(entry);
    }
}

// #------------------------------#
// | static functions definitions |
// #------------------------------#
