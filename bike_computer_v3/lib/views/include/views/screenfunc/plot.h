#ifndef __SCREEN_FUNC_PLOT_DOUBLE_H__
#define __SCREEN_FUNC_PLOT_DOUBLE_H__

#include "../frame.h"
#include "../../../../tools/include/RingBuffer.h"
#include <display/fonts.h>
#include <stdbool.h>



typedef struct PlotData
{
    RingBuffer *buffer;
} PlotData;

/*draws floats from ring buffer*/
void PlotFloatDraw(void *data, void *settings, Frame *plotFrame);
void PlotDrawByte(void *data, void *settings, Frame *plotFrame);

#endif