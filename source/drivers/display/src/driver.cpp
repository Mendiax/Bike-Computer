#include "stdio.h"

#include "display/driver.hpp"
#include "hw_functions.hpp"
#include "traces.h"
// #include "spi_interface.hpp"

#include <cstddef>

static uint8_t display_buffer[DISPLAY_BUFFER_SIZE] = {0};

void display::init(void)
{
    display_init();
}




void display::set_pixel_row(const uint_fast32_t idx, uint_fast32_t width, const display::DisplayColor color)
{
    if ((idx + width) > DISPLAY_PIXEL_COUNT)
    {
        TRACE_ABNORMAL(TRACE_DISPLAY_PRINT,"[ERROR] display::setPixel %" PRIuFAST32 " > DISPLAY_PIXEL_COUNT\n", idx);
        return;
    }
    if (width == 0)
    {
        TRACE_ABNORMAL(TRACE_DISPLAY_PRINT,"[ERROR] display::setPixel width cannot be 0. width == %" PRIuFAST32 " \n", width);
        return;
    }
    struct pixel2
    {
        uint8_t rg;
        uint8_t br;
        uint8_t gb;
    };
    pixel2 *buffer_pixels = (pixel2 *)::display_buffer;

    uint_fast32_t index_pixels = idx >> 1;
    pixel2 * pixel_p = &buffer_pixels[index_pixels];

    // align in memory if needed
    if (idx & 1)
    {
        pixel_p->br = (pixel_p->br & 0xf0) | (color.r & 0x0f);
        pixel_p->gb = (color.g << 4) | (color.b & 0x0f);
        ++pixel_p;
        --width;
    }
    uint_fast32_t pixels_left = width >> 1;
    pixel2 filled_pixel;
    filled_pixel.rg = (color.r << 4) | color.g;
    filled_pixel.br = (color.b << 4) | color.r;
    filled_pixel.gb = (color.g << 4) | color.b;

    // file 2 pixels at time
    while (pixels_left > 0)
    {
        *pixel_p = filled_pixel;
        ++pixel_p;
        --pixels_left;
    }

    // align end of line
    if(width & 1)
    {
        pixel_p->rg = (color.r << 4) | (color.g & 0x0f);
        pixel_p->br = (color.b << 4) | (pixel_p->br & 0x0f);
    }
}


void display::setPixel(const uint_fast32_t idx, const display::DisplayColor color)
{
    // assert_loop((idx) < DISPLAY_PIXEL_COUNT);
    if ((idx) > DISPLAY_PIXEL_COUNT)
    {
        TRACE_ABNORMAL(TRACE_DISPLAY_PRINT, "[ERROR] display::setPixel %" PRIuFAST32 " > DISPLAY_PIXEL_COUNT\n", idx);
        return;
    }
    struct pixel2
    {
        uint8_t rg;
        uint8_t br;
        uint8_t gb;

    };
    pixel2 *buffer_pixels = (pixel2 *)::display_buffer;

    const uint_fast32_t index_pixels = idx >> 1;
    pixel2 *const pixel_p = &buffer_pixels[index_pixels];
    if (idx & 1)
    {
        pixel_p->br = (pixel_p->br & 0xf0) | (color.r & 0x0f);
        pixel_p->gb = (color.g << 4) | (color.b & 0x0f);
    }
    else
    {
        pixel_p->rg = (color.r << 4) | (color.g & 0x0f);
        pixel_p->br = (color.b << 4) | (pixel_p->br & 0x0f);
    }
}

/**
 * @brief Clear screen
 *
 */
void display::clear(void)
{
    memset(::display_buffer, 0x0, DISPLAY_BUFFER_SIZE);
}


/**
 * @brief Update all memory to OLED
 *
 */
void display::display()
{
    display_display(::display_buffer);
}


// static declaration
static void Paint_DrawCharGen(uint16_t x, uint16_t y, const char character,
                              const sFONT *font,
                              display::DisplayColor colorForeground = FONT_FOREGROUND,
                              display::DisplayColor colorBackground = FONT_BACKGROUND,
                              uint8_t scale = 1,
                              bool overwriteBackground = false);

static void Paint_PrintlnGen(uint16_t x, uint16_t y, const char *str,
                             const sFONT *font,
                             display::DisplayColor colorForeground = FONT_FOREGROUND,
                             display::DisplayColor colorBackground = FONT_BACKGROUND,
                             uint8_t scale = 1,
                             bool overwriteBackground = false);

static void Paint_Println_multilineGen(uint16_t x, uint16_t y, const char *str,
                                       const sFONT *font,
                                       display::DisplayColor colorForeground = FONT_FOREGROUND,
                                       display::DisplayColor colorBackground = FONT_BACKGROUND,
                                       uint8_t scale = 1,
                                       bool overwriteBackground = false);

void Paint_DrawLineGen(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, void (*draw_func)(uint16_t x, uint16_t y, display::DisplayColor color), display::DisplayColor color, uint8_t scale);


// global definitions
void display::set_pixel(uint16_t x, uint16_t y, display::DisplayColor color)
{
    /*            width
    #---------------------------->x
    |
    |
    |
    | height
    |
    |
    |
   \/
    y
    */
    if(x >= DISPLAY_WIDTH || y >= DISPLAY_HEIGHT)
    {
        TRACE_ABNORMAL(TRACE_DISPLAY_PRINT, "write outside of window x=%" PRIu16 " y=%" PRIu16 "\n", x, y);
        //return;
        // no return (XD) so it can be optimised out when compiling without traces and have greater performance
    }
    uint_fast32_t index = 0;
    index += y * (display::width);
    index += x;
    display::setPixel(index, color);
}

void display::draw_char(uint16_t x, uint16_t y, const char character,
                    const sFONT *font,
                    display::DisplayColor colorForeground,
                    uint8_t scale)
{
    Paint_DrawCharGen(x, y, character,
                      font,
                      colorForeground,
                      FONT_BACKGROUND,
                      scale,
                      false);
}

void display::println(uint16_t x, uint16_t y, const char *str,
                   const sFONT *font,
                   display::DisplayColor colorForeground,
                   uint8_t scale)
{
    Paint_PrintlnGen(x, y, str,
                     font,
                     colorForeground,
                     FONT_BACKGROUND,
                     scale,
                     false);
}

void display::draw_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, display::DisplayColor color, uint8_t scale)
{
    Paint_DrawLineGen(x0,y0,x1,y1, display::set_pixel, color, scale);
}

void Paint_DrawLineGen(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, void (*draw_func)(uint16_t x, uint16_t y, display::DisplayColor color), display::DisplayColor color, [[maybe_unused]]uint8_t scale)
{
    int_fast32_t dx = std::abs((int32_t)x1 - (int32_t)x0);
    uint_fast8_t sx = x0 < x1 ? 1 : -1;
    int_fast32_t dy = - std::abs((int32_t)y1 - (int32_t)y0);
    uint_fast8_t sy = y0 < y1 ? 1 : -1;

    int_fast32_t error = dx + dy;

    while(true)
    {
        draw_func(x0, y0, color);
        if(x0 == x1 && y0 == y1)
            break;
        auto e2 = 2 * error;
        if (e2 >= dy)
        {
            if(x0 == x1)
                break;
            error = error + dy;
            x0 = x0 + sx;
        }

        if (e2 <= dx)
        {
            if (y0 == y1)
                break;
            error = error + dx;
            y0 = y0 + sy;
        }
    }
}

static void Paint_PrintlnGen(uint16_t x, uint16_t y, const char *str,
                             const sFONT *font, display::DisplayColor colorForeground, display::DisplayColor colorBackground, uint8_t scale, bool overwriteBackground)
{
    TRACE_DEBUG(1, TRACE_DISPLAY_PRINT, "writing string \"%s\" \r\n", str);

    for (size_t charIdx = 0; charIdx < strlen(str); charIdx++)
    {
        if (x > display::width - font->width)
        {
            TRACE_ABNORMAL(TRACE_DISPLAY_PRINT,"line too long %s at %u", str, x);
            return;
        }
        Paint_DrawCharGen(x, y, str[charIdx], font, colorForeground, colorBackground, scale, overwriteBackground);
        x += font->width * scale;
    }
}

[[maybe_unused]]
static void Paint_Println_multilineGen(uint16_t x, uint16_t y, const char *str,
                                       const sFONT *font, display::DisplayColor colorForeground, display::DisplayColor colorBackground, uint8_t scale, bool overwriteBackground)
{
    TRACE_DEBUG(1, TRACE_DISPLAY_PRINT, "writing string \"%s\" \r\n", str);

    uint16_t startX = x;
    for (size_t charIdx = 0; charIdx < strlen(str); charIdx++)
    {
        if (x > display::width - font->width)
        {
            x = startX;
            y += font->height;
        }
        Paint_DrawCharGen(x, y, str[charIdx], font, colorForeground, colorBackground, scale, overwriteBackground);
        x += font->width;
    }
}

static void setpixelScale(uint_fast16_t xPoint, uint_fast16_t yPoint,  uint_fast16_t page, uint_fast16_t column, display::DisplayColor color, uint_fast8_t scale)
{
    uint_fast16_t x = xPoint + column * scale;
    uint_fast16_t y = yPoint + page * scale;

    for (uint_fast16_t p2 = 0; p2 < scale; p2++)
    {
        uint_fast32_t index = 0;
        index += (y + p2) * (display::width);
        index += x;
        display::set_pixel_row(index, scale, color);
    }
}

static void Paint_DrawCharGen(uint16_t xPoint, uint16_t yPoint, const char character,
                              const sFONT *font, display::DisplayColor colorForeground, display::DisplayColor colorBackground, uint8_t scale,
                              bool overwriteBackground)
{
    if(character < 32 || character > 126)
        return;

    TRACE_DEBUG(2, TRACE_DISPLAY_PRINT, "writing char \"%c\" scale %" PRIu8 "\r\n", character, scale);

    uint16_t page, column;
    if (xPoint > display::width || yPoint > display::height)
    {
        TRACE_ABNORMAL(TRACE_DISPLAY_PRINT, " display::draw_char Input exceeds display range %" PRIu16 ",%" PRIu16 "\n", xPoint, yPoint);
        return;
    }

    size_t charOffset = (character - ' ') * font->height * (font->width / 8 + (font->width % 8 ? 1 : 0));
    const unsigned char *ptr = &font->table[charOffset];

    for (page = 0; page < font->height; page++)
    {
        for (column = 0; column < font->width; column++)
        {
            if (*ptr & (0x80 >> (column % 8)))
            {
                setpixelScale(xPoint, yPoint, page, column, colorForeground, scale);
            }
            else if (overwriteBackground)
            {
                setpixelScale(xPoint, yPoint, page, column, colorBackground, scale);
            }
            // One pixel is 8 bits
            if (column % 8 == 7)
                ptr++;
        }
        if (font->width % 8 != 0)
            ptr++;
    }
}
