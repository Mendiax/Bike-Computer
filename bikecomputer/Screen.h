#ifndef __SCREEN_H__
#define __SCREEN_H__

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#include "ringbuffer.h"

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void Screen_Setup()
{
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }
  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(200); // Pause for 2 seconds
}

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
/*draws floats from ring buffer*/
#define Screen_drawPlotRingBuffer(ringbuffer, min, max, type) (                                                                   \
    {                                                                                                                \
      display.clearDisplay();                                                                                        \
      int i = 0;                                                                                                     \
      long next = SCREEN_HEIGHT - map(ring_buffer_get_element_at(ringbuffer, i, type), min, max, 1, SCREEN_HEIGHT); \
      long current;                                                                                                  \
      while (i < SCREEN_WIDTH - 2)                                                                                   \
      {                                                                                                              \
        current = next;                                                                                              \
        i++;                                                                                                         \
        next = SCREEN_HEIGHT - map(ring_buffer_get_element_at(ringbuffer, i, type), min, max, 1, SCREEN_HEIGHT);    \
        display.drawLine(i - 1, current, i, next, SSD1306_WHITE);                                                    \
      }                                                                                                              \
      display.display();                                                                                             \
    })

#endif

void  Screen_drawString(const char* msg){
  display.println(msg);
}