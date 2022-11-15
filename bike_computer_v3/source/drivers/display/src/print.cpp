#include "display/print.h"
#include "display/debug.h"
#include "display/driver.hpp"
#include "traces.h"
#include <string.h>

#define GLOBAL_SCALE 1//240.0 / 128.0
#define SCALE(X) (X = X * GLOBAL_SCALE)

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
void Paint_SetPixel(uint16_t x, uint16_t y, display::DisplayColor color)
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

void Paint_DrawChar(uint16_t x, uint16_t y, const char character,
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

void Paint_Println(uint16_t x, uint16_t y, const char *str,
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

void Paint_Println_multiline(uint16_t x, uint16_t y, const char *str,
                             const sFONT *font,
                             display::DisplayColor colorForeground,
                             uint8_t scale)
{
    Paint_Println_multilineGen(x, y, str,
                               font,
                               colorForeground,
                               FONT_BACKGROUND,
                               scale,
                               false);
}

void Paint_DrawCharBackground(uint16_t x, uint16_t y, const char character,
                              const sFONT *font,
                              display::DisplayColor colorForeground,
                              display::DisplayColor colorBackground,
                              uint8_t scale)
{
    Paint_DrawCharGen(x, y, character,
                      font,
                      colorForeground,
                      colorBackground,
                      scale,
                      true);
}

void Paint_PrintlnBackground(uint16_t x, uint16_t y, const char *str,
                             const sFONT *font,
                             display::DisplayColor colorForeground,
                             display::DisplayColor colorBackground,
                             uint8_t scale)
{
    Paint_PrintlnGen(x, y, str,
                     font,
                     colorForeground,
                     colorBackground,
                     scale,
                     true);
}

void Paint_Println_multilineBackground(uint16_t x, uint16_t y, const char *str,
                                       const sFONT *font,
                                       display::DisplayColor colorForeground,
                                       display::DisplayColor colorBackground,
                                       uint8_t scale)
{
    Paint_Println_multilineGen(x, y, str,
                               font,
                               colorForeground,
                               colorBackground,
                               scale,
                               true);
}

void Paint_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, display::DisplayColor color, uint8_t scale)
{
    Paint_DrawLineGen(x0,y0,x1,y1, Paint_SetPixel, color, scale);
}

// static definitions
void Paint_DrawLineGen(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, void (*draw_func)(uint16_t x, uint16_t y, display::DisplayColor color), display::DisplayColor color, uint8_t scale)
{
    // source https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
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

    for (int charIdx = 0; charIdx < strlen(str); charIdx++)
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
        // for (uint16_t p = 0; p < scale; p++)
        // {
        //     // uint16_t x = xPoint + column * scale + p;
        //     // uint16_t y = yPoint + page * scale + p2;
        //     Paint_SetPixel(x + p, y + p2, color);
        // }
    }
}

static void Paint_DrawCharGen(uint16_t xPoint, uint16_t yPoint, const char character,
                              const sFONT *font, display::DisplayColor colorForeground, display::DisplayColor colorBackground, uint8_t scale,
                              bool overwriteBackground)
{
    if(character < 32 || character > 126)
        return;
    xPoint *= GLOBAL_SCALE;
    yPoint *= GLOBAL_SCALE;
    scale *= GLOBAL_SCALE;

    TRACE_DEBUG(2, TRACE_DISPLAY_PRINT, "writing char \"%c\" scale %" PRIu8 "\r\n", character, scale);

    //DEBUG_OLED("writing char\'%c\'\n", character);
    uint16_t page, column;
    if (xPoint > display::width || yPoint > display::height)
    {
        TRACE_ABNORMAL(TRACE_DISPLAY_PRINT, " Paint_DrawChar Input exceeds display range %" PRIu16 ",%" PRIu16 "\n", xPoint, yPoint);
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
            else if (overwriteBackground) // To determine whether the font background color and screen background color is consistent
            {                             // this process is to speed up the scan
                setpixelScale(xPoint, yPoint, page, column, colorBackground, scale);
            }
            // One pixel is 8 bits
            if (column % 8 == 7)
                ptr++;
        } // Write a line
        if (font->width % 8 != 0)
            ptr++;
    } // Write all
}
