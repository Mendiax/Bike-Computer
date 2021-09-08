#ifndef __SCREEN_FUNC_VAL_H__
#define __SCREEN_FUNC_VAL_H__

#include "../../addons/ringbuffer.h"
#include "../screen.h"
#include "../../addons/print.h"
#include <stdio.h>

typedef struct ValSettings
{
    char* format;
    unsigned maxLength;
    unsigned textSize;
    unsigned offsetX, offsetY;
} ValSettings;

typedef struct ValData
{
    double val; // must be double
} ValData;

/*draws last val from ring buffer*/
void ValDraw(void *data, void *settings, Frame *plotFrame)
{
    ValSettings *valSettings = (ValSettings *)settings;
    ValData* plotData = (ValData*)data;
    
    int value = plotData->val;
    
    unsigned maxStrLen = valSettings->maxLength + 1; 
    char buffer[maxStrLen + 1];

    int write = snprintf(&buffer[0], maxStrLen, valSettings->format, value);

    maxStrLen = min(maxStrLen,write);
    
    display.setTextSize(valSettings->textSize);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(plotFrame->x + valSettings->offsetX, valSettings->offsetY);
    display.cp437(true);


    for (unsigned i = 0; i < maxStrLen; i++)
    {
        display.write(buffer[i]);
    }
}
#endif
