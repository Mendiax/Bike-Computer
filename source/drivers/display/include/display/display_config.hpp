#ifndef __DISPLAY_DISPLAY_CONFIG_HPP__
#define __DISPLAY_DISPLAY_CONFIG_HPP__
// #-------------------------------#
// |           includes            |
// #-------------------------------#
// pico includes

// c/c++ includes

// my includes

// #-------------------------------#
// |            macros             |
// #-------------------------------#
//#define ROTATE
#ifdef ROTATE
    #define DISPLAY_WIDTH  240
    #define DISPLAY_HEIGHT 320
#else
    #define DISPLAY_HEIGHT 240
    #define DISPLAY_WIDTH  320
#endif

// SETUP
#ifdef ROTATE
    // VERITCAL
    #define MY 1
    #define MX 1
    #define MV 0
#else
    // HORIZONTAL
    #define MY 0
    #define MX 1
    #define MV 1
#endif

#define ML 0
#define RGB_ 0
#define MH 1

// 12bit per color so
// reg 3Ah
#define INPUT_COLORS 2
#if INPUT_COLORS == 0 // 5-6-5 bit
#define REG_3Ah 0x05
#define PIXEL_SIZE 16
#elif INPUT_COLORS == 1 // 6-6-6 bit
#define REG_3Ah 0x06
#define PIXEL_SIZE 18
#elif INPUT_COLORS == 2 // 4-4-4 bit
#define REG_3Ah 0x03
#define PIXEL_SIZE 12
#endif

#define DISPLAY_PIXEL_COUNT (DISPLAY_HEIGHT * DISPLAY_WIDTH)

// #-------------------------------#
// | global types declarations     |
// #-------------------------------#

// #-------------------------------#
// | global variables declarations |
// #-------------------------------#

// #-------------------------------#
// | global function declarations  |
// #-------------------------------#

#endif
