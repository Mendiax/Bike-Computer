#ifndef __MULTICORE_MUTEX_HPP__
#define __MULTICORE_MUTEX_HPP__
// #-------------------------------#
// |           includes            |
// #-------------------------------#
// pico includes

// c/c++ includes
#include <mutex>

// my includes

// #-------------------------------#
// |            macros             |
// #-------------------------------#

// #-------------------------------#
// | global types declarations     |
// #-------------------------------#
using mutex = std::mutex;
// #-------------------------------#
// | global variables declarations |
// #-------------------------------#

// #-------------------------------#
// | global function declarations  |
// #-------------------------------#
void mutex_init(mutex* mtx_p);
void mutex_enter_blocking(mutex* mtx_p);
void mutex_exit(mutex* mtx_p);


#endif
