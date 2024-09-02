#ifndef __DISPLAY_DRIVER_H__
#define __DISPLAY_DRIVER_H__

#include <stdlib.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include <string.h>

#include "display/fonts.h"
#include "display/display_config.hpp"

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



/* colors ------------------------------------------------------------------*/
#define COLOR_WHITE          (display::DisplayColor){0xF, 0xF, 0xF}
#define COLOR_BLACK          (display::DisplayColor){0x00, 0x0, 0x0}
#define COLOR_GRAY           (display::DisplayColor){0X03, 0x3, 0x3}

#define IMAGE_BACKGROUND    COLOR_BLACK
#define FONT_FOREGROUND     COLOR_WHITE
#define FONT_BACKGROUND     COLOR_BLACK

/* Fonts ------------------------------------------------------------------*/



/* Print function ------------------------------------------------------------------*/


namespace display
{
    const size_t width = DISPLAY_WIDTH;
    const size_t height = DISPLAY_HEIGHT;

    struct DisplayColor
    {
        uint8_t r, g, b;
    };

    void init(void);
    void clear(void);
    void setPixel(const size_t idx, const DisplayColor color);
    void set_pixel_row(const uint_fast32_t idx, const uint_fast32_t width, const display::DisplayColor color);
    void display();

    void draw_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, display::DisplayColor color = FONT_FOREGROUND, uint8_t scale = 1);

    void set_pixel(uint16_t x, uint16_t y, display::DisplayColor color);

    void draw_char(uint16_t x, uint16_t y, const char character,
                   const sFONT* font,
                   display::DisplayColor colorForeground = FONT_FOREGROUND,
                   uint8_t scale = 1);

    void println(uint16_t x, uint16_t y, const char* str,
                 const sFONT* font,
                 display::DisplayColor colorForeground = FONT_FOREGROUND,
                 uint8_t scale = 1);
}
#endif
