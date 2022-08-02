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
        // no return so it can be optimised out when compiling without traces and have greater performance
    }
    size_t index = 0;
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

// static definitions
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

static void setpixelScale(uint16_t xPoint, uint16_t yPoint,  uint16_t page, uint16_t column, display::DisplayColor color, uint8_t scale)
{
    for (uint16_t p = 0; p < scale; p++)
    {
        for (uint16_t p2 = 0; p2 < scale; p2++)
        {
            uint16_t x = xPoint + column * scale + p;
            uint16_t y = yPoint + page * scale + p2;
            Paint_SetPixel(x, y, color);
        }
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
        TRACE_ABNORMAL(TRACE_DISPLAY_PRINT, "[ERROR] Paint_DrawChar Input exceeds the normal display range\n");
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
