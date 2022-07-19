#include <views/screenfunc/lastval.h>
#include "views/frame.h"
#include <stdio.h>
#include <display/print.h>

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

