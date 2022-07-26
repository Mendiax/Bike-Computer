#include <views/screenfunc/val.h>
#include <display/fonts.h>
#include "views/frame.h"
#include <stdio.h>
#include <display/print.h>


template<typename T>
void drawFormatVariable(void *data, void *settings, Frame *plotFrame);

void drawFormatInt8(void *data, void *settings, Frame *plotFrame)
{
    drawFormatVariable<int8_t>(data, settings, plotFrame);
}
void drawFormatInt16(void *data, void *settings, Frame *plotFrame)
{
    drawFormatVariable<int16_t>(data, settings, plotFrame);
}
void drawFormatInt32(void *data, void *settings, Frame *plotFrame)
{
    drawFormatVariable<int32_t>(data, settings, plotFrame);
}
void drawFormatInt64(void *data, void *settings, Frame *plotFrame)
{
    drawFormatVariable<int64_t>(data, settings, plotFrame);
}
void drawFormatFloat(void *data, void *settings, Frame *plotFrame)
{
    drawFormatVariable<float>(data, settings, plotFrame);
}
void drawFormatDouble(void *data, void *settings, Frame *plotFrame)
{
    drawFormatVariable<double>(data, settings, plotFrame);
}


template<typename T>
void drawFormatVariable(void *data, void *settings, Frame *plotFrame)
{
    ValSettings *valSettings = (ValSettings *)settings;

    T value = *((T *)data);

    size_t maxStrLen = valSettings->maxLength + 1;
    char buffer[maxStrLen + 1];

    int write = snprintf(&buffer[0], maxStrLen, valSettings->format, value);

    if(write < 0){
        return;
    }

    int x = plotFrame->x + valSettings->offsetX;
    int y = plotFrame->y + valSettings->offsetY;
    Paint_Println(x, y, buffer, valSettings->textSize, COLOR_WHITE, valSettings->textScale);
}

void ValDrawTime(void *data, void *settings, Frame *plotFrame)
{
    ValSettings *valSettings = (ValSettings *)settings;
    ValDataULong* time = (ValDataULong *)data;

    char hours = time->val / 3600;
    char min = (time->val % 3600) / 60;
    char sec = time->val % 60; 


    unsigned maxStrLen = valSettings->maxLength + 1;
    char buffer[maxStrLen + 1];

    int write = snprintf(&buffer[0], maxStrLen, valSettings->format, hours, min, sec);

    if(write < 0){
        return;
    }

    int x = plotFrame->x + valSettings->offsetX;
    int y = plotFrame->y +valSettings->offsetY;
    Paint_Println(x, y, buffer, valSettings->textSize, COLOR_WHITE, valSettings->textScale);
}
