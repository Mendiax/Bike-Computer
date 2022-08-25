#include <views/screenfunc/plot.h>
#include "views/frame.h"
#include <display/fonts.h>
#include <stdbool.h>
#include "views/screenfunc/common.h"
#include <iostream>

#include <display/print.h>
#include "massert.h"

// drawFunc_p get_draw_func_plot(float* arr)
// {
//     return plot_float;
// }

template<typename T, typename O>
static inline O map(const T& val, const T& min, const T& max, const O& min_o, const O& max_o)
{
    //std::cout << "val_o: " << val_o << " " << val << " " << min << " " << max << " " << min_o << " " << max_o  << std::endl;
    // test input
    if(val < min)
    {
        return min_o;
    }
    if(val > max)
    {
        return max_o;
    }
    O val_o = (float)((float)(max_o - min_o) / (float)(max - min)) * val + min_o;
    // test output
    if(val_o < min_o)
    {
        return min_o;
    }
    if(val_o > max_o)
    {
        return max_o;
    }
    return val_o; // not tested
    
}

void plot_float(void *settings)
{
    typedef float f_t;
    PlotSettings* plot_settings = (PlotSettings*)settings;

    //auto offset = plot_settings->offset == NULL ? 0 : *plot_settings->offset;
    f_t* array_p = (f_t*)plot_settings->data;// + offset;
    f_t min_val, max_val;

    if(plot_settings->auto_max || plot_settings->auto_min)
    {
        //take max of array
        for(uint_fast16_t i = 0; i< plot_settings->len; ++i)
        {
            max_val = std::max(max_val, array_p[i]);
            min_val = std::max(min_val, array_p[i]);
        }
        if(min_val == max_val)
        {
            max_val = min_val + 1;
        }
    }

    min_val = plot_settings->auto_min ? min_val : *(f_t*) plot_settings->min;
    max_val = plot_settings->auto_max ? max_val : *(f_t*) plot_settings->max;

    uint16_t dx = (plot_settings->frame.width - 1) / (plot_settings->len-1);
    uint_fast16_t x0,y0,x1,y1;
    uint_fast16_t fr_min_y = plot_settings->frame.y;
    uint_fast16_t fr_max_y = fr_min_y + plot_settings->frame.height;

    //std::cout << "frame: " << plot_settings->frame.y << " " << plot_settings->frame.height << " " << fr_min_y + plot_settings->frame.height << std::endl;
    //std::cout << "dx: " << dx << " " <<fr_min_y << " " << fr_max_y << " " << min_val << " " << max_val << std::endl;

    x0 = plot_settings->frame.x;
    massert( min_val != max_val && fr_min_y != fr_max_y, "division by 0 \n");
    y0 = fr_max_y - map(array_p[0], min_val, max_val, (uint16_t)0, plot_settings->frame.height);

    for(uint_fast16_t i = 1; i< plot_settings->len; ++i)
    {
        //std::cout << array_p[i] << std::endl;
        //render plot
        x1 = x0 + dx;
        y1 = fr_max_y - map(array_p[i], min_val, max_val, (uint16_t)0, plot_settings->frame.height);
        //std::cout << "i: " << i << " " << x0 << " " << y0 << " " << x1 << " " << y1 << std::endl;

        Paint_DrawLine(x0,y0,x1,y1, plot_settings->color);
        x0 = x1;
        y0 = y1;
    }
}

// /*draws floats from ring buffer*/
// void PlotFloatDraw(void *data, void *settings, Frame *plotFrame)
// {
//     //DEBUG_PRINT("plots update");
//     /*PlotSettings *plot = (PlotSettings *)settings;
//     PlotData plotData = {(RingBuffer *)data};
//     const size_t MAX_X = plotFrame->x + plotFrame->width;//, MAX_Y = plotFrame->y + plotFrame->height;

//     size_t x_cord = plotFrame->x;
//     size_t buf_pos = -1 * plotFrame->width;
//     long nextValue = SCREEN_HEIGHT - map((long)ring_buffer_get_element_at_float(plotData.buffer, buf_pos), plot->min, plot->max, 1, plotFrame->height - 1);
//     long currentValue;
//     while (x_cord < MAX_X - 2)
//     {
//         currentValue = nextValue;
//         x_cord++;
//         nextValue = SCREEN_HEIGHT - map((long)ring_buffer_get_element_at_float(plotData.buffer, buf_pos), plot->min, plot->max, 1, plotFrame->height - 1);
//         buf_pos++;
//         //display.drawLine(x_cord - 1, currentValue, x_cord, nextValue, SSD1306_WHITE);
//     }*/
// }

// void PlotDrawByte(void *data, void *settings, Frame *plotFrame)
// {
//     //DEBUG_PRINT("plots update");
//     /*PlotSettings *plot = (PlotSettings *)settings;
//     PlotData plotData = {(RingBuffer *)data};
//     const size_t MAX_X = plotFrame->x + plotFrame->width;//, MAX_Y = plotFrame->y + plotFrame->height;

//     size_t x_cord = plotFrame->x;
//     size_t buf_pos = -1 * plotFrame->width;

//     long minVal = plot->min;
//     long maxVal = plot->max;

//     if(plot->autoMax){
//         maxVal = ring_buffer_get_max(plotData.buffer) + 5;
//     }
//     if(plot->autoMin){
//         minVal = ring_buffer_get_min(plotData.buffer) - 5;
//     }
//     long nextValue = SCREEN_HEIGHT - map((long)ring_buffer_get_element_at_byte(plotData.buffer, buf_pos), plot->min, plot->max, 1, plotFrame->height - 1);
//     long currentValue;
//     while (x_cord < MAX_X - 2)
//     {
//         currentValue = nextValue;
//         x_cord++;
//         nextValue = SCREEN_HEIGHT - map((long)ring_buffer_get_element_at_byte(plotData.buffer, buf_pos), plot->min, plot->max, 1, plotFrame->height - 1);
//         buf_pos++;
//         //display.drawLine(x_cord - 1, currentValue, x_cord, nextValue, SSD1306_WHITE);
//     }*/
// }
