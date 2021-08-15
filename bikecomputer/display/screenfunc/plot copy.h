#ifndef __SCREEN_FUNC_OTHER_H__
#define __SCREEN_FUNC_OTHER_H__

/*draws floats from ring buffer*/
void Screen_drawPlotAtPosRingBuffer(ringbuffer, min, max, type,x0,y0,height,width) (                              \                                      \
    {                                                                                                                \
      const byte MAX_X = x0 + width, MAX_Y = y0 + height;                                                            \                                      \
      display.clearDisplay();                                                                                        \
      int i = x0;                                                                                                    \
      long next = MAX_Y - map(ring_buffer_get_element_at(ringbuffer, i, type), min, max, y0, MAX_Y);                 \
      long current;                                                                                                  \
      while (i < MAX_X - 2)                                                                                          \
      {                                                                                                              \
        current = next;                                                                                              \
        i++;                                                                                                         \
        next = MAX_Y - map(ring_buffer_get_element_at(ringbuffer, i, type), min, max, y0, MAX_Y);                    \
        display.drawLine(i - 1, current, i, next, SSD1306_WHITE);                                                    \
      }                                                                                                              \
      display.display();                                                                                             \
    })

#endif