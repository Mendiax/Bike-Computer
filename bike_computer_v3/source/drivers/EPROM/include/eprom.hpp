#ifndef EPROM_HPP
#define EPROM_HPP
// #-------------------------------#
// |           includes            |
// #-------------------------------#
// pico includes

// c/c++ includes
#include <stdint.h>

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

namespace eprom
{
    uint8_t read_byte(uint8_t address);
    void write_byte(uint8_t address, uint8_t byte);
};

// #-------------------------------#
// | global function declarations  |
// #-------------------------------#

#endif
