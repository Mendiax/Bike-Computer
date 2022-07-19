#ifndef __SCREEN_H__
#define __SCREEN_H__

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 128 // OLED display height, in pixels

#define SCREEN_HEIGHT_YELLOW 8
#define SCREEN_HEIGHT_BLUE 24

void Screen_setup();
void Screen_draw();
void Screen_clear();


#endif