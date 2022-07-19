#include <views/screenfunc/val.h>
#include <display/fonts.h>
#include "views/frame.h"
#include <stdio.h>
#include <display/print.h>


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
    Paint_Println(x, y, buffer, valSettings->textSize, COLOR_WHITE, valSettings->textScale);
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
