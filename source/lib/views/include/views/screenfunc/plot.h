#ifndef __SCREEN_FUNC_PLOT_DOUBLE_H__
#define __SCREEN_FUNC_PLOT_DOUBLE_H__

#include "frame.h"
#include <display/fonts.h>
#include <stdbool.h>
#include "massert.hpp"
#include "ringarray.hpp"
#include "ringbuffer.h"
#include "traces.h"
#include "views/screenfunc/common.h"


#define DECLARE_FUNC(type) \
    void drawFormat_##type(const void *settings); \
    DrawFuncT get_draw_func_plot(const type* var);


template<typename T>
void plot_array(const void *settings);
template<typename T>
void plot_ringarray(const void *settings);

void plot_arrayminmax(const void *settings);

template<typename T>
DrawFuncT get_draw_func_plot()
{
    return plot_array<T>;
}

template<typename T>
DrawFuncT get_draw_func_plot_ring()
{
    return plot_ringarray<T>;
}


template<typename T, typename O>
static inline O map(const T& val, const T& min, const T& max, const O& min_o, const O& max_o)
{
    //std::cout << "val_o: " << val_o << " " << val << " " << min << " " << max << " " << min_o << " " << max_o  << std::endl;
    // test input
    if (min == max)
    {
        return max_o;  // or max_o; since min == max, both represent the same point
    }
    if(val < min)
    {
        return min_o;
    }
    if(val > max)
    {
        return max_o;
    }
    O val_o = static_cast<O>((static_cast<double>(val - min) / static_cast<double>(max - min)) * static_cast<double>(max_o - min_o) + min_o);

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

// typedef struct PlotData
// {
//     RingBuffer *buffer;
// } PlotData;

// /*draws floats from ring buffer*/
// void PlotFloatDraw(void *data, void *settings, Frame *plotFrame);
// void PlotDrawByte(void *data, void *settings, Frame *plotFrame);
template<typename T>
void plot_ringarray(const void *settings)
{
    // typedef float T;
    PlotSettings* plot_settings = (PlotSettings*)settings;

    //auto offset = plot_settings->offset == NULL ? 0 : *plot_settings->offset;
    RingArray<T>& ring_array = *((RingArray<T>*)plot_settings->data);// + offset;


    T min_val = 0, max_val = 0;
    if(plot_settings->auto_max || plot_settings->auto_min)
    {
        //take max of array
        auto data = ring_array.get_data();
        for(uint_fast16_t i = 0; i< data.size(); ++i)
        {
            max_val = std::max(max_val, data.at(i));
            min_val = std::min(min_val, data.at(i));
        }
        if(min_val == max_val)
        {
            max_val = min_val + 1;
        }
    }

    min_val = plot_settings->auto_min ? min_val : *((T*) plot_settings->min);
    max_val = plot_settings->auto_max ? max_val : *((T*) plot_settings->max);

    uint16_t dx = (plot_settings->frame.width - 1) / (plot_settings->len-1);
    uint_fast16_t x0,y0,x1,y1;
    uint_fast16_t fr_min_y = plot_settings->frame.y;
    uint_fast16_t fr_max_y = fr_min_y + plot_settings->frame.height - 1;
    const uint16_t map_max = plot_settings->frame.height - 1;
    const uint16_t map_min = 0;


    //std::cout << "frame: " << plot_settings->frame.y << " " << plot_settings->frame.height << " " << fr_min_y + plot_settings->frame.height << std::endl;
    //std::cout << "dx: " << dx << " " <<fr_min_y << " " << fr_max_y << " " << min_val << " " << max_val << std::endl;
    auto iter = ring_array.begin();
    x0 = plot_settings->frame.x;
    massert( min_val != max_val && fr_min_y != fr_max_y, "division by 0 \n");
    y0 = fr_max_y - map(*iter, min_val, max_val, map_min, map_max);
    ++iter;

    for(;iter != ring_array.end(); ++iter)
     {
        //std::cout << array_p[i] << std::endl;
        //render plot
        x1 = x0 + dx;
        y1 = fr_max_y - map(*iter, min_val, max_val, map_min, map_max);
        //std::cout << "i: " << i << " " << x0 << " " << y0 << " " << x1 << " " << y1 << std::endl;
        // massert_range((uint16_t)y0, plot_settings->frame.y, plot_settings->frame.y + plot_settings->frame.height - 1);
        // massert_range((uint16_t)y1, plot_settings->frame.y, plot_settings->frame.y + plot_settings->frame.height - 1);
        // massert_range((uint16_t)x0, plot_settings->frame.x, plot_settings->frame.x + plot_settings->frame.width - 1);
        // massert_range((uint16_t)x1, plot_settings->frame.x, plot_settings->frame.x + plot_settings->frame.width - 1);


        display::draw_line(x0,y0,x1,y1, plot_settings->color);
        x0 = x1;
        y0 = y1;
    }
}

template<typename T>
void plot_array(const void *settings)
{
    // typedef float T;
    PlotSettings* plot_settings = (PlotSettings*)settings;

    //auto offset = plot_settings->offset == NULL ? 0 : *plot_settings->offset;
    T* array_p = (T*)plot_settings->data;// + offset;
    T min_val = 0.0, max_val = 0.0;

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

    min_val = plot_settings->auto_min ? min_val : *(T*) plot_settings->min;
    max_val = plot_settings->auto_max ? max_val : *(T*) plot_settings->max;

    uint16_t dx = (plot_settings->frame.width - 1) / (plot_settings->len-1);
    uint_fast16_t x0,y0,x1,y1;
    uint_fast16_t fr_min_y = plot_settings->frame.y;
    uint_fast16_t fr_max_y = fr_min_y + plot_settings->frame.height - 1;
    const uint16_t map_max = plot_settings->frame.height - 1;
    const uint16_t map_min = 0;


    //std::cout << "frame: " << plot_settings->frame.y << " " << plot_settings->frame.height << " " << fr_min_y + plot_settings->frame.height << std::endl;
    //std::cout << "dx: " << dx << " " <<fr_min_y << " " << fr_max_y << " " << min_val << " " << max_val << std::endl;

    x0 = plot_settings->frame.x;
    massert( min_val != max_val && fr_min_y != fr_max_y, "division by 0 \n");
    y0 = fr_max_y - map(array_p[0], min_val, max_val, map_min, map_max);

    for(uint_fast16_t i = 1; i< plot_settings->len; ++i)
    {
        //std::cout << array_p[i] << std::endl;
        //render plot
        x1 = x0 + dx;
        y1 = fr_max_y - map(array_p[i], min_val, max_val, map_min, map_max);
        //std::cout << "i: " << i << " " << x0 << " " << y0 << " " << x1 << " " << y1 << std::endl;
        // massert_range((uint16_t)y0, plot_settings->frame.y, plot_settings->frame.y + plot_settings->frame.height - 1);
        // massert_range((uint16_t)y1, plot_settings->frame.y, plot_settings->frame.y + plot_settings->frame.height - 1);
        // massert_range((uint16_t)x0, plot_settings->frame.x, plot_settings->frame.x + plot_settings->frame.width - 1);
        // massert_range((uint16_t)x1, plot_settings->frame.x, plot_settings->frame.x + plot_settings->frame.width - 1);


        display::draw_line(x0,y0,x1,y1, plot_settings->color);
        x0 = x1;
        y0 = y1;
    }
}

#endif