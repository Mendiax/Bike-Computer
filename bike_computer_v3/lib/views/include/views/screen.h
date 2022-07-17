#ifndef __SCREEN_H__
#define __SCREEN_H__

#include "display/print.h"
#include "../../../tools/include/RingBuffer.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 128 // OLED display height, in pixels

#define SCREEN_HEIGHT_YELLOW 8
#define SCREEN_HEIGHT_BLUE 24


void Screen_setup()
{
  display::init();
  display::clear();
  Paint_Println(0,10, "START", &Font24, FONT_FOREGROUND);
  display::display();
}

typedef struct Frame
{
    uint8_t x, y, width, height;
} Frame;

void Screen_draw(){
  display::display();
}

void Screen_clear(){
  display::clear();
  //__display->clear();
}


#endif