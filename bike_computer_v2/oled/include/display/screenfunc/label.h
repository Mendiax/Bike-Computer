#ifndef __SCREEN_FUNC_LABEL_H__
#define __SCREEN_FUNC_LABEL_H__

#include "../screen.h"
#include <stdio.h>

typedef struct LabelSettings
{
    const char* string;
    sFONT* textSize;
    unsigned offsetX, offsetY;
} LabelSettings;

typedef struct LabelData
{
    
} LabelData;

/*print string*/
void LabelDraw(void *data, void *settings, Frame *plotFrame)
{   
    LabelSettings *labelSettings = (LabelSettings *)settings;

    int x = plotFrame->x + labelSettings->offsetX;
    int y = plotFrame->y + labelSettings->offsetY;
    Paint_Println(&__display, x, y, labelSettings->string, labelSettings->textSize, 0x0f, 0x00);
}
#endif
