#ifndef __SCREEN_FUNC_LASTVAL_H__
#define __SCREEN_FUNC_LASTVAL_H__

#include "../../addons/ringbuffer.h"
#include "../screen.h"
#include "../../addons/print.h"
#include <stdio.h>

typedef struct LastValSettings
{
    bool isInt;
    unsigned maxLength;
    unsigned textSize;
    unsigned offsetX, offsetY;
} LastValSettings;

typedef struct LastValData
{
    RingBuffer *bufferDouble; // must be double
} LastValData;

/*draws last val from ring buffer*/
void LastValDraw(void *data, void *settings, Frame *plotFrame)
{
    //DEBUG_PRINT("plots update");
    LastValSettings *lastValSettings = (LastValSettings *)settings;
    LastValData plotData = {(RingBuffer *)data};
    const size_t MAX_X = plotFrame->x + plotFrame->width, MAX_Y = plotFrame->y + plotFrame->height;
    
    int value = *(double*)ring_buffer_get_last_element_pointer(plotData.bufferDouble);
    //Serial.println(value);
    
    unsigned maxStrLen = lastValSettings->maxLength; 
    char buffer[maxStrLen + 1];

    int write = snprintf(&buffer[0], maxStrLen, "%2d", value);

    maxStrLen = min(maxStrLen,write);
    
    display.setTextSize(lastValSettings->textSize);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(plotFrame->x + lastValSettings->offsetX, lastValSettings->offsetY);
    display.cp437(true);

    //Serial.println(maxStrLen);
    //Serial.println(buffer);


    for (unsigned i = 0; i < maxStrLen; i++)
    {
        display.write(buffer[i]);
    }
}
#endif
