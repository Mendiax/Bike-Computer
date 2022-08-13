#ifndef __DISPLAY_DRIVER_H__
#define __DISPLAY_DRIVER_H__

#include <stdlib.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include <string.h>

#define LOW 0
#define HIGH 1

// OLED
//#define SPI_PORT spi0
// #define RST 7
// #define DC 6
// #define CS   5
// #define MISO 4 // not used
// #define MOSI 3
// #define CLK  2

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
    #define RGB 0
    #define MH 1

#define DISPLAY_PIXEL_COUNT (DISPLAY_HEIGHT * DISPLAY_WIDTH)

namespace display
{
    const size_t width = DISPLAY_WIDTH;
    const size_t height = DISPLAY_HEIGHT;
    extern uint8_t display_buffer[];

    struct DisplayColor
    {
    public:
        uint16_t color;
        int8_t r, g, b;
        DisplayColor(int8_t r, int8_t g, int8_t b)
            : r(r & 0xf), g(g & 0xf), b(b & 0xf)
        {
            // uint8_t max = std::max(std::max(r,g), b);

            color =
                (((uint16_t)r << 8) & 0xf00) |
                (((uint16_t)g << 4) & 0x0f0) |
                (((uint16_t)b << 0) & 0x00f);
        }
    };

    void init(void);
    // TODO add param with color
    void clear(void);
    void fill(const DisplayColor color);
    void fill(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const DisplayColor color);
    void setPixel(const size_t idx, const DisplayColor color);
    void set_pixel_row(const uint_fast32_t idx, const uint_fast32_t width, const display::DisplayColor color);
    void display();
}
#endif
