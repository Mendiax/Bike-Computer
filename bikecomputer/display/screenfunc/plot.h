#ifndef __SCREEN_FUNC_PLOT_DOUBLE_H__
#define __SCREEN_FUNC_PLOT_DOUBLE_H__

#include "../../addons/ringbuffer.h"
#include "../screen.h"
#include "../../addons/print.h"

typedef struct PlotFloatSettings
{
    long min, max;
} PlotFloatSettings;

typedef struct PlotFloatData
{
    RingBuffer *bufferFloat;
} PlotFloatData;

/*draws floats from ring buffer*/
void PlotDoubleDraw(void *data, void *settings, Frame *plotFrame)
{
    //DEBUG_PRINT("plots update");
    PlotFloatSettings *plot = (PlotFloatSettings *)settings;
    PlotFloatData plotData = {(RingBuffer *)data};
    const size_t MAX_X = plotFrame->x + plotFrame->width, MAX_Y = plotFrame->y + plotFrame->height;

    size_t x_cord = plotFrame->x;
    size_t buf_pos = -1 * plotFrame->width;
    long nextValue = SCREEN_HEIGHT - map((long)ring_buffer_get_element_at_float(plotData.bufferFloat, buf_pos), plot->min, plot->max, 1, plotFrame->height - 1);
    long currentValue;
    while (x_cord < MAX_X - 2)
    {
        currentValue = nextValue;
        x_cord++;
        nextValue = SCREEN_HEIGHT - map((long)ring_buffer_get_element_at_float(plotData.bufferFloat, buf_pos), plot->min, plot->max, 1, plotFrame->height - 1);
        buf_pos++;
        display.drawLine(x_cord - 1, currentValue, x_cord, nextValue, SSD1306_WHITE);
    }
}
#endif