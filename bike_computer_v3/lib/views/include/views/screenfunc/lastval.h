#ifndef __SCREEN_FUNC_LASTVAL_H__
#define __SCREEN_FUNC_LASTVAL_H__

#include "../screen.h"
#include <stdio.h>

typedef struct LastValSettings
{
    const char* format;
    unsigned maxLength;
    const sFONT* textSize;
    uint8_t textScale;
    unsigned offsetX, offsetY;
} LastValSettings;

typedef struct LastValData
{
    RingBuffer *buffer; // must be double
} LastValData;

/*draws last val from ring buffer*/
void LastValDraw(void *data, void *settings, Frame *plotFrame)
{
    //DEBUG_PRINT("plots update");
    LastValSettings *lastValSettings = (LastValSettings *)settings;
    LastValData plotData = {(RingBuffer *)data};
    
    int value = *(float*)ring_buffer_get_last_element_pointer(plotData.buffer);
    //Serial.println(value);
    
    unsigned maxStrLen = lastValSettings->maxLength + 1; 
    char buffer[maxStrLen + 1];

    int write = snprintf(&buffer[0], maxStrLen, lastValSettings->format, value);

    if(write < 0){
        return;
    }

    int x = plotFrame->x + lastValSettings->offsetX;
    int y = plotFrame->y + lastValSettings->offsetY;
    Paint_Println(x, y, buffer, lastValSettings->textSize,COLOR_WHITE, lastValSettings->textScale);
}

void LastValDrawByte(void *data, void *settings, Frame *plotFrame)
{
    //DEBUG_PRINT("plots update");
    LastValSettings *lastValSettings = (LastValSettings *)settings;
    LastValData plotData = {(RingBuffer *)data};

    int value = *(char*)ring_buffer_get_last_element_pointer(plotData.buffer);//ring_buffer_get_last_element(plotData.buffer);

    int maxStrLen = lastValSettings->maxLength + 1; 
    char buffer[maxStrLen + 1]; // VLA xd

    int write = snprintf(&buffer[0], maxStrLen, lastValSettings->format, value);

    if(write < 0){
        return;
    }

    int x = plotFrame->x + lastValSettings->offsetX;
    int y = plotFrame->y + lastValSettings->offsetY;
    Paint_Println(x, y, buffer, lastValSettings->textSize,COLOR_WHITE, lastValSettings->textScale);
}
#endif
