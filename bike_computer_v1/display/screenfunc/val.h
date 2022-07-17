#ifndef __SCREEN_FUNC_VAL_H__
#define __SCREEN_FUNC_VAL_H__

#include "../../addons/ringbuffer.h"
#include "../screen.h"
#include "../../addons/print.h"
#include <stdio.h>

typedef struct ValSettings
{
    const char *format;
    unsigned maxLength;
    unsigned textSize;
    unsigned offsetX, offsetY;
} ValSettings;

typedef struct ValData
{
    float val;
} ValData;

typedef struct ValDataInt
{
    int val;
} ValDataInt;

typedef struct ValDataULong
{
    unsigned long val;
} ValDataULong;

/*draws last val from ring buffer*/
void ValDrawInt(void *data, void *settings, Frame *plotFrame)
{
    ValSettings *valSettings = (ValSettings *)settings;
    ValDataInt *plotData = (ValDataInt *)data;

    int value = plotData->val;

    unsigned maxStrLen = valSettings->maxLength + 1;
    char buffer[maxStrLen + 1];

    int write = snprintf(&buffer[0], maxStrLen, valSettings->format, value);

    if(write < 0){
        return;
    }

    maxStrLen = min(maxStrLen, (unsigned)write);

    display.setTextSize(valSettings->textSize);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(plotFrame->x + valSettings->offsetX, valSettings->offsetY);
    display.cp437(true);

    for (unsigned i = 0; i < maxStrLen; i++)
    {
        display.write(buffer[i]);
    }
}

/*draws last val from ring buffer*/
void ValDraw(void *data, void *settings, Frame *plotFrame)
{
    ValSettings *valSettings = (ValSettings *)settings;
    ValData *plotData = (ValData *)data;

    int value = plotData->val;

    unsigned maxStrLen = valSettings->maxLength + 1;
    char buffer[maxStrLen + 1];

    int write = snprintf(&buffer[0], maxStrLen, valSettings->format, value);

    if(write < 0){
        return;
    }

    maxStrLen = min(maxStrLen, (unsigned)write);

    display.setTextSize(valSettings->textSize);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(plotFrame->x + valSettings->offsetX, valSettings->offsetY);
    display.cp437(true);

    for (unsigned i = 0; i < maxStrLen; i++)
    {
        display.write(buffer[i]);
    }
}

void ValDrawTime(void *data, void *settings, Frame *plotFrame)
{
    ValSettings *valSettings = (ValSettings *)settings;
    ValDataULong* time = (ValDataULong *)data;

    byte hours = time->val / 3600;
    byte min = (time->val % 3600) / 60;
    byte sec = time->val % 60; 


    unsigned maxStrLen = valSettings->maxLength + 1;
    char buffer[maxStrLen + 1];

    int write = snprintf(&buffer[0], maxStrLen, valSettings->format, hours, min, sec);

    if(write < 0){
        return;
    }

    maxStrLen = min(maxStrLen, (unsigned)write);

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
