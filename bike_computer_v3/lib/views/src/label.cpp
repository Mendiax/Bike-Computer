
#include <views/screenfunc/label.h>
#include "views/frame.h"
#include <stdio.h>
#include <display/print.h>
#include "views/screenfunc/common.h"
#include "traces.h"


/*print string*/
void LabelDraw(void *data, void *settings, Frame *plotFrame)
{   
    LabelSettings *labelSettings = (LabelSettings *)settings;

    int x = plotFrame->x + labelSettings->text.offsetX;
    int y = plotFrame->y + labelSettings->text.offsetY;
    TRACE_DEBUG(0, TRACE_DISPLAY_PRINT, "Drawing label %s at [%d, %d]\n", labelSettings->text.string, x, y);
    Paint_Println(x, y, labelSettings->text.string, labelSettings->text.font, COLOR_WHITE, labelSettings->text.scale);
    TRACE_DEBUG(0, TRACE_DISPLAY_PRINT, "finished label %s at [%d, %d]\n", labelSettings->text.string, x, y);

}

