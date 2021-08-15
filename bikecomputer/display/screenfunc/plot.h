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
void PlotDoubleDraw(void *data, void *settings, Frame *window)
{
    //DEBUG_PRINT("plots update");
    PlotDoubleSettings *plot = (PlotDoubleSettings *)settings;
    PlotDoubleData plotData = {(RingBuffer *)data};
    const byte MAX_X = window->x + window->width, MAX_Y = window->y + window->height;

    byte x_cord = window->x;
    byte buf_pos = plotData.bufferDouble->current_index - window->width;
    long next = SCREEN_HEIGHT - map((long)ring_buffer_get_element_at_double(plotData.bufferDouble, buf_pos), plot->min, plot->max, 0, window->height);
    long current;
    while (x_cord < MAX_X - 2)
    {
        current = next;
        x_cord++;
        next = SCREEN_HEIGHT - map((long)ring_buffer_get_element_at_double(plotData.bufferDouble, buf_pos), plot->min, plot->max, 0, window->height);
        buf_pos++;
        display.drawLine(x_cord - 1, current, x_cord, next, SSD1306_WHITE);
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