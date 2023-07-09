#ifndef __DISPLAY_HW_FUNCTIONS_HPP__
#define __DISPLAY_HW_FUNCTIONS_HPP__
// #-------------------------------#
// |           includes            |
// #-------------------------------#
// pico includes

// c/c++ includes
#include <stdint.h>

// my includes
#include "display/display_config.hpp"

// #-------------------------------#
// |            macros             |
// #-------------------------------#


#define DISPLAY_PIXEL_COUNT (DISPLAY_HEIGHT * DISPLAY_WIDTH)
#define DISPLAY_BUFFER_SIZE (DISPLAY_PIXEL_COUNT * PIXEL_SIZE / 8)
// #-------------------------------#
// | global types declarations     |
// #-------------------------------#

// #-------------------------------#
// | global variables declarations |
// #-------------------------------#

// #-------------------------------#
// | global function declarations  |
// #-------------------------------#
void display_init(void);
void display_display(uint8_t display_buffer[DISPLAY_BUFFER_SIZE]);

#endif
