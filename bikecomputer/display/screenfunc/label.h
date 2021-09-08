#ifndef __SCREEN_FUNC_LABEL_H__
#define __SCREEN_FUNC_LABEL_H__

#include "../screen.h"
#include "../../addons/print.h"
#include <stdio.h>

typedef struct LabelSettings
{
    char* string;
    unsigned textSize;
    unsigned offsetX, offsetY;
} LabelSettings;

typedef struct LabelData
{
    
} LabelData;

/*print string*/
void LabelDraw(void *data, void *settings, Frame *plotFrame)
{
    LabelSettings *labelSettings = (LabelSettings *)settings;
    
    unsigned maxStrLen = strlen(labelSettings->string);
    
    display.setTextSize(labelSettings->textSize);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(plotFrame->x + labelSettings->offsetX, labelSettings->offsetY);
    display.cp437(true);


    for (unsigned i = 0; i < maxStrLen; i++)
    {
        display.write(labelSettings->string[i]);
    }
}
#endif
