#ifndef __SCREEN_FUNC_VAL_H__
#define __SCREEN_FUNC_VAL_H__


#include "../screen.h"
#include <stdio.h>
#include <fonts.h>

typedef struct ValSettings
{
    const char *format;
    unsigned maxLength;
    sFONT* textSize;
    uint8_t textScale;
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

    int x = plotFrame->x + valSettings->offsetX;
    int y = plotFrame->y + valSettings->offsetY;
    Paint_Println(&__display, x, y, buffer, valSettings->textSize, 0x0f, 0x00, valSettings->textScale);
}


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

    int x = plotFrame->x + valSettings->offsetX;
    int y = plotFrame->y + valSettings->offsetY;
    Paint_Println(&__display, x, y, buffer, valSettings->textSize, 0x0f, 0x00, valSettings->textScale);
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
    Paint_Println(&__display, x, y, buffer, valSettings->textSize, 0x0f, 0x00, valSettings->textScale);
}
#endif
