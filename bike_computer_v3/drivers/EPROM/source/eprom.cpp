
// #------------------------------#
// |          includes            |
// #------------------------------#
// pico includes
#include "hardware/i2c.h"

// c/c++ includes

// my includes
#include "eprom.hpp"
#include "I2C.h"

// #------------------------------#
// |           macros             |
// #------------------------------#
#define WRITE_ADDR 10100000
#define READ_ADDR 10100001


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
uint8_t eprom::read_byte(uint8_t address)
{
	uint8_t TempVal = 0;
	i2c_write_blocking(i2c1, WRITE_ADDR, &address, 1, true);  // true to keep master control of bus
    // read in one go as register addresses auto-increment
    i2c_read_blocking(i2c1, READ_ADDR, &TempVal, 1, false);  // false, we're done reading
	return TempVal;
}
void eprom::write_byte(uint8_t address, uint8_t byte)
{
    uint8_t packet[] = {address, byte};
    i2c_write_blocking(i2c1, WRITE_ADDR, packet, sizeof(packet), false);
}

// #------------------------------#
// | static functions definitions |
// #------------------------------#
