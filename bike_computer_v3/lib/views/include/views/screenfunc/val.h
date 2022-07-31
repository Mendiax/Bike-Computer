#ifndef __SCREEN_FUNC_VAL_H__
#define __SCREEN_FUNC_VAL_H__

#include "../../../../tools/include/RingBuffer.h"
#include <display/fonts.h>
#include "../frame.h"
#include <stdio.h>

typedef struct ValDataULong
{
    unsigned long val;
} ValDataULong;

/*draws last val from ring buffer*/
void drawFormatInt8(void *data, void *settings, Frame *plotFrame);
void drawFormatInt16(void *data, void *settings, Frame *plotFrame);
void drawFormatInt32(void *data, void *settings, Frame *plotFrame);
void drawFormatInt64(void *data, void *settings, Frame *plotFrame);
void drawFormatFloat(void *data, void *settings, Frame *plotFrame);
void drawFormatDouble(void *data, void *settings, Frame *plotFrame);

void ValDrawTime(void *data, void *settings, Frame *plotFrame);

#endif
