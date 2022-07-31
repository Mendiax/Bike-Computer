#ifndef SCREENFUNC_COMMON_H
#define SCREENFUNC_COMMON_H

#include "display/fonts.h"

/* data ptr, additional data ptr, window(size) */
typedef void (*drawFunc_p)(void *, void *, Frame *);

/* class containing part of data to draw */
struct DisplayData
{
    const char* format;
    uint16_t format_length;
    void* data;
    drawFunc_p func;
};

// should be first in the struct
typedef struct TextSettings
{
    const char* string;
    const sFONT* font;
    uint16_t str_len;
    uint16_t offsetX, offsetY;
    uint8_t scale;
} TextSettings;

typedef struct LabelSettings
{
    TextSettings text; // but str_len should be set only for scaling
} LabelSettings;

typedef struct LastValSettings
{
    TextSettings text;
} LastValSettings;

typedef struct PlotSettings
{
    long min, max;
    bool autoMax;
    bool autoMin;
    long offset;
} PlotSettings;

typedef struct ValSettings
{
    TextSettings text;
} ValSettings;

#endif
