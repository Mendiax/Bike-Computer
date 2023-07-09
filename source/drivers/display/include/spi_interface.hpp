#ifndef __DISPLAY_SPI_INTERFACE_HPP__
#define __DISPLAY_SPI_INTERFACE_HPP__
// #-------------------------------#
// |           includes            |
// #-------------------------------#
// pico includes

// c/c++ includes
#include <stdio.h>

// my includes

// #-------------------------------#
// |            macros             |
// #-------------------------------#

// #-------------------------------#
// | global types declarations     |
// #-------------------------------#

// #-------------------------------#
// | global variables declarations |
// #-------------------------------#

// #-------------------------------#
// | global function declarations  |
// #-------------------------------#
namespace spi
{
    uint8_t init(void);
    void writeData(uint8_t Data);
    void writeRegister(uint8_t Reg);
    void writeDataBuffer(const uint8_t *Data, size_t length);
    void reset(void);
    void set_brightness(uint8_t Value);
}

#endif
