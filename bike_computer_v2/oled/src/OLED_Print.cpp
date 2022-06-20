#include <OLED_Print.h>
#include <OLED_Driver.h>
#include <OLED_DEBUG.h>
#include <string.h>

#include <fonts.h>

void Paint_Println(DisplayBuffer* display, uint8_t x, uint8_t y, const char* str,
                    sFONT* font, uint8_t colorForeground, uint8_t colorBackground, uint8_t scale)
{
    //DEBUG_OLED("writing string \"%s\"\n", str);
    for(int charIdx = 0; charIdx < strlen(str); charIdx++)
    {
        if(x > display->width - font->width)
        {
            DEBUG_OLED("line too long %s at %u", str, x);
            return;
        }
        Paint_DrawChar(display, x, y, str[charIdx], font, colorForeground, colorBackground, scale);
        x += font->width * scale;
    }
}

void Paint_Println_multiline(DisplayBuffer* display, uint8_t x, uint8_t y, const char* str,
                            sFONT* font, uint8_t colorForeground, uint8_t colorBackground, uint8_t scale)
{
    //DEBUG_OLED("writing string \"%s\"\n", str);
    uint8_t startX = x;
    for(int charIdx = 0; charIdx < strlen(str); charIdx++)
    {
        if(x > display->width - font->width)
        {
            x = startX;
            y += font->height;
        }
        Paint_DrawChar(display, x, y, str[charIdx], font, colorForeground, colorBackground, scale);
        x += font->width;
    }
}

void Paint_DrawChar(DisplayBuffer* display, uint8_t xPoint, uint8_t yPoint, const char character,
                           sFONT* font, uint8_t colorForeground, uint8_t colorBackground, uint8_t scale,
                           bool overwriteBackground)
{
    //DEBUG_OLED("writing char\'%c\'\n", character);
    uint8_t page, column;

    if (xPoint > display->width || yPoint > display->height) {
        //Debug("Paint_DrawChar Input exceeds the normal display range\r\n");
        return;
    }

    uint32_t charOffset = (character - ' ') * font->height * (font->width / 8 + (font->width % 8 ? 1 : 0));
    const unsigned char *ptr = &font->table[charOffset];

    for (page = 0; page < font->height; page ++ ) {
        for (column = 0; column < font->width; column ++ ) {
            
            for(uint8_t p = 0; p < scale; p++)
            {
                for(uint8_t p2 = 0; p2 < scale; p2++)
                {
                    uint8_t x = xPoint + column * scale + p;
                    uint8_t y = yPoint + page * scale + p2;
                    if (*ptr & (0x80 >> (column % 8)))
                    {
                        Paint_SetPixel(display, x , y , colorForeground);
                    }
                    else if (overwriteBackground) //To determine whether the font background color and screen background color is consistent
                    { //this process is to speed up the scan
                        Paint_SetPixel(display, x, y, colorBackground);
                    }
                }
            }
            //One pixel is 8 bits
            if (column % 8 == 7)
                ptr++;
        }// Write a line
        if (font->width % 8 != 0)
            ptr++;
    }// Write all
}

inline void Paint_SetPixel(DisplayBuffer* display, uint8_t x, uint8_t y, uint8_t color)
{
    if(x >= display->width || y >= display->height){return;} // TODOD might add #if debug to not waste cycles
    uint8_t colorMask = 0x0f;
    color = color & colorMask;
    uint8_t clear = 0xf0;
    uint64_t index = 0;
    index += y * (display->width/2);
    index += (x / 2);
    if(x % 2 == 0) // it is upper 4bits
    {
        color = color << 4;
        clear = 0x0f;
    }
    display->buffer[index] &= clear;
    display->buffer[index] |= color;
}