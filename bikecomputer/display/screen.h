#ifndef __SCREEN_H__
#define __SCREEN_H__

//#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define SCREEN_HEIGHT_YELLOW 8
#define SCREEN_HEIGHT_BLUE 24

#include "../addons/ringbuffer.h"

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void Screen_setup()
{
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  { // Address 0x3C for 128x32
    Serial.println(F("[SCREEN] SSD1306 allocation failed"));
    for (;;);
  }
  display.display();
  delay(200);
}

typedef struct Frame
{
    byte x, y, width, height;
} Frame;

void Screen_draw(){
  display.display();
}

void Screen_clear(){
  display.clearDisplay();
}


/*
byte newArray[SCREEN_WIDTH];
void Screen_drawPlot(double _array[], byte startpoint)
{
  byte Position = 0;
  for (byte i = startpoint; i < SCREEN_WIDTH; i++)
  {
    newArray[Position++] = SCREEN_HEIGHT - map(_array[i], 0, 100, 1, SCREEN_HEIGHT);
  }
  for (byte i = 0; i < startpoint; i++)
  {
    newArray[Position++] = SCREEN_HEIGHT - map(_array[i], 0, 100, 1, SCREEN_HEIGHT);
  }
  display.clearDisplay();
  int i = 0;
  while (i < SCREEN_WIDTH - 2)
  {
    display.drawLine(i, newArray[i], i + 1, newArray[i + 1], SSD1306_WHITE);
    i++;
  }
  //  while (i < SCREEN_WIDTH - 1)
  //  {
  //    display.drawPixel(i, newArray[i++], SSD1306_WHITE);
  //  }
  display.display();
}
*/
/*draws floats from ring buffer full screenm*/
/*#define Screen_drawPlotRingBuffer(ringbuffer, min, max, type) (                                                                   \
    {                                                                                                                \
      Screen_drawPlotAtPosRingBuffer(ringbuffer, min, max, type, 0, 0, SCREEN_HEIGHT, SCREEN_WIDTH);                                                                                           \
    })


void  Screen_drawString(const char* msg){
  display.println(msg);
}*/

#endif