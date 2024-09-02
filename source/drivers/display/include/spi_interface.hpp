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
    void write_data(uint8_t Data);
    void write_register(uint8_t Reg);
    void write_data_buffer(const uint8_t *Data, size_t length);
    void write_data_buffer_dma(uint dma_tx, const uint8_t *Data, size_t length);

    void reset(void);
    void set_brightness(uint8_t Value);
}

#endif
