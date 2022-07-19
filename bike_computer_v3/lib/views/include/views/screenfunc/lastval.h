#ifndef __SCREEN_FUNC_LASTVAL_H__
#define __SCREEN_FUNC_LASTVAL_H__

#include "../frame.h"
#include <stdio.h>
#include "../../../../tools/include/RingBuffer.h"
#include <display/fonts.h>

typedef struct LastValSettings
{
    const char* format;
    unsigned maxLength;
    const sFONT* textSize;
    uint8_t textScale;
    unsigned offsetX, offsetY;
} LastValSettings;

typedef struct LastValData
{
    RingBuffer *buffer; // must be double
} LastValData;

/*draws last val from ring buffer*/
void LastValDraw(void *data, void *settings, Frame *plotFrame);
void LastValDrawByte(void *data, void *settings, Frame *plotFrame);

#endif
