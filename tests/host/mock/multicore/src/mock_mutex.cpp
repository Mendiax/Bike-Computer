
// #------------------------------#
// |          includes            |
// #------------------------------#
// pico includes

// c/c++ includes

// my includes
#include "mock_mutex.hpp"
#include <cassert>

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
void mutex_init(mutex* mtx_p)
{
    assert(mtx_p);
    mtx_p = new mutex;
}

void mutex_enter_blocking(mutex* mtx_p)
{
    assert(mtx_p);
    mtx_p->lock();
}
void mutex_exit(mutex* mtx_p)
{
    assert(mtx_p);
    mtx_p->unlock();
}
// #------------------------------#
// | static functions definitions |
// #------------------------------#
