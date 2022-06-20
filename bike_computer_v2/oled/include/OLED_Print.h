#ifndef __OLED_PRINT_H__
#define __OLED_PRINT_H__

#include <inttypes.h>
#include <OLED_Driver.h>
#include <fonts.h>
#include <string>

#define COLOR_WHITE          0x0F
#define COLOR_BLACK          0x00
#define COLOR_GRAY           0X03

#define IMAGE_BACKGROUND    COLOR_BLACK
#define FONT_FOREGROUND     COLOR_WHITE
#define FONT_BACKGROUND     COLOR_BLACK

void Paint_DrawChar(DisplayBuffer* display, uint8_t x, uint8_t y, const char character,
                    sFONT* font, uint8_t colorForeground = FONT_FOREGROUND, uint8_t colorBackground = FONT_BACKGROUND, uint8_t scale = 1, bool overwriteBackground = false);

inline void Paint_SetPixel(DisplayBuffer* display, uint8_t x, uint8_t y, uint8_t Color);

void Paint_Println(DisplayBuffer* display, uint8_t x, uint8_t y, const char* str,
                    sFONT* font, uint8_t colorForeground = FONT_FOREGROUND, uint8_t colorBackground = FONT_BACKGROUND, uint8_t scale = 1);

void Paint_Println_multiline(DisplayBuffer* display, uint8_t x, uint8_t y, const char* str,
                            sFONT* font, uint8_t colorForeground = FONT_FOREGROUND, uint8_t colorBackground = FONT_BACKGROUND, uint8_t scale = 1);

#endif