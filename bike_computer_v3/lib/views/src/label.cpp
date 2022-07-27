
#include <views/screenfunc/label.h>
#include "views/frame.h"
#include <stdio.h>
#include <display/print.h>


/*print string*/
void LabelDraw(void *data, void *settings, Frame *plotFrame)
{   
    LabelSettings *labelSettings = (LabelSettings *)settings;

    int x = plotFrame->x + labelSettings->offsetX;
    int y = plotFrame->y + labelSettings->offsetY;
    Paint_Println(x, y, labelSettings->string, labelSettings->textSize, COLOR_WHITE, labelSettings->scale);
}

