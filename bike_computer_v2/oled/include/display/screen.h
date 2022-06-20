#ifndef __SCREEN_H__
#define __SCREEN_H__

#include <OLED_Print.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 128 // OLED display height, in pixels

#define SCREEN_HEIGHT_YELLOW 8
#define SCREEN_HEIGHT_BLUE 24

#include "../../../tools/include/RingBuffer.h"


static DisplayBuffer __display(128, 128);

void Screen_setup()
{
  OLED_1in5_Init();
  OLED_1in5_Clear();
  Paint_Println(&__display, 0,10, "START", &Font24, 0x0f, 0x00);
  __display.write();
}

typedef struct Frame
{
    uint8_t x, y, width, height;
} Frame;

void Screen_draw(){
  __display.write();
}

void Screen_clear(){
  //OLED_1in5_Clear();
  __display.clear();
}


#endif