#ifndef __SCREEN_FUNC_PLOT_DOUBLE_H__
#define __SCREEN_FUNC_PLOT_DOUBLE_H__

#include "../../addons/ringbuffer.h"
#include "../screen.h"
#include "../../addons/print.h"

typedef struct PlotDoubleSettings
{
    long min, max;
} PlotDoubleSettings;

typedef struct PlotDoubleData
{
    RingBuffer *bufferDouble; // must be double
} PlotDoubleData;

/*draws floats from ring buffer*/
void PlotDoubleDraw(void *data, void *settings, Frame *plotFrame)
{
    //DEBUG_PRINT("plots update");
    PlotDoubleSettings *plot = (PlotDoubleSettings *)settings;
    PlotDoubleData plotData = {(RingBuffer *)data};
    const size_t MAX_X = plotFrame->x + plotFrame->width, MAX_Y = plotFrame->y + plotFrame->height;

    size_t x_cord = plotFrame->x;
    size_t buf_pos = -1 * plotFrame->width;
    long nextValue = SCREEN_HEIGHT - map((long)ring_buffer_get_element_at_double(plotData.bufferDouble, buf_pos), plot->min, plot->max, 1, plotFrame->height - 1);
    long currentValue;
    while (x_cord < MAX_X - 2)
    {
        currentValue = nextValue;
        x_cord++;
        nextValue = SCREEN_HEIGHT - map((long)ring_buffer_get_element_at_double(plotData.bufferDouble, buf_pos), plot->min, plot->max, 1, plotFrame->height - 1);
        buf_pos++;
        display.drawLine(x_cord - 1, currentValue, x_cord, nextValue, SSD1306_WHITE);
    }
}

/*void PlotDoubleDraw(void *data, void *settings, Frame *window)
{
    //DEBUG_PRINT("plots update");
    PlotDoubleSettings *plot = (PlotDoubleSettings *)settings;
    PlotDoubleData plotData = {(RingBuffer *)data};
    const byte MAX_X = window->x + window->width, MAX_Y = window->y + window->height;

    byte i = window->x;
    long next = MAX_Y - map((long)ring_buffer_get_element_at(plotData.bufferDouble, i, double), plot->min, plot->max, window->y, MAX_Y);
    long current;
    while (i < MAX_X - 2)
    {
        current = next;
        i++;
        next = MAX_Y - map((long)ring_buffer_get_element_at(plotData.bufferDouble, i, double), plot->min, plot->max, window->y, MAX_Y);
        display.drawLine(i - 1, current, i, next, SSD1306_WHITE);
    }
}*/
#endif