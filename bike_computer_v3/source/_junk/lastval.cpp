#include <views/screenfunc/lastval.h>
#include "views/frame.h"
#include <stdio.h>
#include <display/print.h>
#include "views/screenfunc/common.h"

/*draws last val from ring buffer*/
void LastValDraw(void *data, void *settings, Frame *plotFrame)
{
    // //DEBUG_PRINT("plots update");
    // LastValSettings *lastValSettings = (LastValSettings *)settings;
    // LastValData plotData = {(RingBuffer *)data};

    // int value = *(float*)ring_buffer_get_last_element_pointer(plotData.buffer);
    // //Serial.println(value);

    // uint16_t maxStrLen = lastValSettings->text.str_len + 1;
    // char buffer[maxStrLen + 1];

    // int write = snprintf(&buffer[0], maxStrLen, lastValSettings->text.string, value);

    // if(write < 0){
    //     return;
    // }

    // int x = plotFrame->x + lastValSettings->text.offsetX;
    // int y = plotFrame->y + lastValSettings->text.offsetY;
    // Paint_Println(x, y, buffer, lastValSettings->text.font,COLOR_WHITE, lastValSettings->text.scale);
}

void LastValDrawByte(void *data, void *settings, Frame *plotFrame)
{
    // //DEBUG_PRINT("plots update");
    // LastValSettings *lastValSettings = (LastValSettings *)settings;
    // LastValData plotData = {(RingBuffer *)data};

    // int value = *(char*)ring_buffer_get_last_element_pointer(plotData.buffer);//ring_buffer_get_last_element(plotData.buffer);

    // int maxStrLen = lastValSettings->text.str_len + 1;
    // char buffer[maxStrLen + 1]; // VLA xd

    // int write = snprintf(&buffer[0], maxStrLen, lastValSettings->text.string, value);

    // if(write < 0){
    //     return;
    // }

    // int x = plotFrame->x + lastValSettings->text.offsetX;
    // int y = plotFrame->y + lastValSettings->text.offsetY;
    // Paint_Println(x, y, buffer, lastValSettings->text.font,COLOR_WHITE, lastValSettings->text.scale);
}