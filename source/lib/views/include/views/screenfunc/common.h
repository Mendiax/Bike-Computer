#ifndef SCREENFUNC_COMMON_H
#define SCREENFUNC_COMMON_H

#include "views/frame.h"
#include "display/fonts.h"
#include "display/driver.hpp"
#include "traces.h"

/* data ptr, additional data ptr, window(size) */
typedef void (*DrawFuncT)(const void *);

enum class Align{
    LEFT,
    CENTER,
    RIGHT
};




/* class containing part of data to draw */
struct DisplayData
{
    const char* format;
    uint16_t format_length;
    const void* data;
    DrawFuncT func;
};

// should be first in the struct
typedef struct TextSettings
{
    const char* string;
    const sFONT* font;
    uint16_t str_len;
    uint16_t offsetX, offsetY;
    uint8_t scale;
    bool render_frame;
    const display::DisplayColor* color;
} TextSettings;




// types for drawing funtions

typedef struct LabelSettings
{
    TextSettings text; // but str_len should be set only for scaling
    display::DisplayColor color;
} LabelSettings;

typedef struct ValSettings
{
    TextSettings text;
    const void *data;
} ValSettings;

typedef struct PlotSettings
{
    Frame frame;
    bool auto_max;
    bool auto_min;
    void * min, *max;
    const void *data; // ptr to array
    uint16_t len; // len of drawing mus be less than data
    //uint16_t* offset; // movable offset null if not used
    display::DisplayColor color;
} PlotSettings;

struct FrameSettings
{
    Frame frame;
    display::DisplayColor color;
};

struct Settings
{
    union{
        LabelSettings label;
        ValSettings val;
        PlotSettings plot;
        FrameSettings frame;
    };
};


// ========== unused =============
typedef struct LastValSettings
{
    TextSettings text;
} LastValSettings;

// typedef struct PlotSettings
// {
//     long min, max;
//     bool autoMax;
//     bool autoMin;
//     long offset;
// } PlotSettings;



#endif
