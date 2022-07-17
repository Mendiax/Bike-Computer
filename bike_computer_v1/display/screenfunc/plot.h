#ifndef __SCREEN_FUNC_PLOT_DOUBLE_H__
#define __SCREEN_FUNC_PLOT_DOUBLE_H__

#include "../../addons/ringbuffer.h"
#include "../screen.h"
#include "../../addons/print.h"

typedef struct PlotSettings
{
    long min, max;
    bool autoMax;
    bool autoMin;
    long offset;
} PlotSettings;

typedef struct PlotData
{
    RingBuffer *buffer;
} PlotData;

/*draws floats from ring buffer*/
void PlotFloatDraw(void *data, void *settings, Frame *plotFrame)
{
    //DEBUG_PRINT("plots update");
    PlotSettings *plot = (PlotSettings *)settings;
    PlotData plotData = {(RingBuffer *)data};
    const size_t MAX_X = plotFrame->x + plotFrame->width;//, MAX_Y = plotFrame->y + plotFrame->height;

    size_t x_cord = plotFrame->x;
    size_t buf_pos = -1 * plotFrame->width;
    long nextValue = SCREEN_HEIGHT - map((long)ring_buffer_get_element_at_float(plotData.buffer, buf_pos), plot->min, plot->max, 1, plotFrame->height - 1);
    long currentValue;
    while (x_cord < MAX_X - 2)
    {
        currentValue = nextValue;
        x_cord++;
        nextValue = SCREEN_HEIGHT - map((long)ring_buffer_get_element_at_float(plotData.buffer, buf_pos), plot->min, plot->max, 1, plotFrame->height - 1);
        buf_pos++;
        display.drawLine(x_cord - 1, currentValue, x_cord, nextValue, SSD1306_WHITE);
    }
}

void PlotDrawByte(void *data, void *settings, Frame *plotFrame)
{
    //DEBUG_PRINT("plots update");
    PlotSettings *plot = (PlotSettings *)settings;
    PlotData plotData = {(RingBuffer *)data};
    const size_t MAX_X = plotFrame->x + plotFrame->width;//, MAX_Y = plotFrame->y + plotFrame->height;

    size_t x_cord = plotFrame->x;
    size_t buf_pos = -1 * plotFrame->width;

    long minVal = plot->min;
    long maxVal = plot->max;

    if(plot->autoMax){
        maxVal = ring_buffer_get_max(plotData.buffer) + 5;
    }
    if(plot->autoMin){
        minVal = ring_buffer_get_min(plotData.buffer) - 5;
    }
    long nextValue = SCREEN_HEIGHT - map((long)ring_buffer_get_element_at_byte(plotData.buffer, buf_pos), plot->min, plot->max, 1, plotFrame->height - 1);
    long currentValue;
    while (x_cord < MAX_X - 2)
    {
        currentValue = nextValue;
        x_cord++;
        nextValue = SCREEN_HEIGHT - map((long)ring_buffer_get_element_at_byte(plotData.buffer, buf_pos), plot->min, plot->max, 1, plotFrame->height - 1);
        buf_pos++;
        display.drawLine(x_cord - 1, currentValue, x_cord, nextValue, SSD1306_WHITE);
    }
}

#endif