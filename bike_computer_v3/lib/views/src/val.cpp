#include <views/screenfunc/val.h>
#include <display/fonts.h>
#include "views/frame.h"
#include <stdio.h>
#include <display/print.h>
#include "views/screenfunc/common.h"

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

    size_t maxStrLen = valSettings->text.str_len + 1;
    char buffer[maxStrLen + 1];

    int write = snprintf(&buffer[0], maxStrLen, valSettings->text.string, value);

    if(write < 0){
        return;
    }

    int x = plotFrame->x + valSettings->text.offsetX;
    int y = plotFrame->y + valSettings->text.offsetY;
    Paint_Println(x, y, buffer, valSettings->text.font, COLOR_WHITE, valSettings->text.scale);
}

void ValDrawTime(void *data, void *settings, Frame *plotFrame)
{
    ValSettings *valSettings = (ValSettings *)settings;
    ValDataULong* time = (ValDataULong *)data;

    char hours = time->val / 3600;
    char min = (time->val % 3600) / 60;
    char sec = time->val % 60; 


    unsigned maxStrLen = valSettings->text.str_len + 1;
    char buffer[maxStrLen + 1];

    int write = snprintf(&buffer[0], maxStrLen, valSettings->text.string, hours, min, sec);

    if(write < 0){
        return;
    }

    int x = plotFrame->x + valSettings->text.offsetX;
    int y = plotFrame->y +valSettings->text.offsetY;
    Paint_Println(x, y, buffer, valSettings->text.font, COLOR_WHITE, valSettings->text.scale);
}
