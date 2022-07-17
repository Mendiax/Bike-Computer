#ifndef __OLED_PRINT_H__
#define __OLED_PRINT_H__

#include <inttypes.h>
#include "display/driver.hpp"
#include <string>
#include "display/fonts.h"

/* colors ------------------------------------------------------------------*/
#define COLOR_WHITE          (display::DisplayColor){0xF, 0xF, 0xF}
#define COLOR_BLACK          (display::DisplayColor){0x00, 0x0, 0x0}
#define COLOR_GRAY           (display::DisplayColor){0X03, 0x3, 0x3}

#define IMAGE_BACKGROUND    COLOR_BLACK
#define FONT_FOREGROUND     COLOR_WHITE
#define FONT_BACKGROUND     COLOR_BLACK

/* Fonts ------------------------------------------------------------------*/



/* Print function ------------------------------------------------------------------*/
void Paint_SetPixel(uint16_t x, uint16_t y, display::DisplayColor color);

void Paint_DrawChar(uint16_t x, uint16_t y, const char character,
                    const sFONT* font, 
                    display::DisplayColor colorForeground = FONT_FOREGROUND,
                    uint8_t scale = 1);

void Paint_Println(uint16_t x, uint16_t y, const char* str,
                   const sFONT* font,
                   display::DisplayColor colorForeground = FONT_FOREGROUND,
                   uint8_t scale = 1);

void Paint_Println_multiline(uint16_t x, uint16_t y, const char* str,
                            const sFONT* font,
                            display::DisplayColor colorForeground = FONT_FOREGROUND,
                            uint8_t scale = 1);


void Paint_DrawCharBackground(uint16_t x, uint16_t y, const char character,
                              const sFONT* font, 
                              display::DisplayColor colorForeground = FONT_FOREGROUND,
                              display::DisplayColor colorBackground = FONT_BACKGROUND,
                              uint8_t scale = 1, 
                              bool overwriteBackground = true);

void Paint_PrintlnBackground(uint16_t x, uint16_t y, const char* str,
                            const sFONT* font,
                            display::DisplayColor colorForeground = FONT_FOREGROUND,
                            display::DisplayColor colorBackground = FONT_BACKGROUND,
                            uint8_t scale = 1);

void Paint_Println_multilineBackground(uint16_t x, uint16_t y, const char* str,
                                        const sFONT* font,
                                        display::DisplayColor colorForeground = FONT_FOREGROUND,
                                        display::DisplayColor colorBackground = FONT_BACKGROUND,
                                        uint8_t scale = 1);
#endif