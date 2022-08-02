#ifndef __SCREEN_FUNC_VAL_H__
#define __SCREEN_FUNC_VAL_H__

#include "../../../../tools/include/RingBuffer.h"
#include <display/fonts.h>
#include "../frame.h"
#include <stdio.h>
#include "views/window.h"

/*draws last val from ring buffer*/

#define DECLARE_FUNC(type) \
    void drawFormat_##type(void *settings); \
    drawFunc_p getDrawFunc(type* var);

DECLARE_FUNC(int8_t)
DECLARE_FUNC(uint8_t)
DECLARE_FUNC(uint16_t)
DECLARE_FUNC(int16_t)
DECLARE_FUNC(uint32_t)
DECLARE_FUNC(int32_t)
DECLARE_FUNC(uint64_t)
DECLARE_FUNC(int64_t)
DECLARE_FUNC(float)
DECLARE_FUNC(double)


void drawFormat_void(void *settings);
drawFunc_p getDrawFunc(void* var);

#undef DECLARE_FUNC

void ValDrawTime(void *settings);

#endif
