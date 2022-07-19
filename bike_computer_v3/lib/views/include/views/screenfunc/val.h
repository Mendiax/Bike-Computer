#ifndef __SCREEN_FUNC_VAL_H__
#define __SCREEN_FUNC_VAL_H__

#include "../../../../tools/include/RingBuffer.h"
#include <display/fonts.h>
#include "../frame.h"
#include <stdio.h>

typedef struct ValSettings
{
    const char *format;
    unsigned maxLength;
    const sFONT* textSize;
    uint8_t textScale;
    unsigned offsetX, offsetY;
} ValSettings;

typedef struct ValData
{
    float val;
} ValData;

typedef struct ValDataInt
{
    int val;
} ValDataInt;

typedef struct ValDataULong
{
    unsigned long val;
} ValDataULong;

/*draws last val from ring buffer*/
void ValDrawInt(void *data, void *settings, Frame *plotFrame);
void ValDraw(void *data, void *settings, Frame *plotFrame);
void ValDrawTime(void *data, void *settings, Frame *plotFrame);

#endif
