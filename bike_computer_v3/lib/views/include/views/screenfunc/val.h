#ifndef __SCREEN_FUNC_VAL_H__
#define __SCREEN_FUNC_VAL_H__

// #include "../../../../tools/include/RingBuffer.h"
#include <display/fonts.h>
#include "../frame.h"
#include <stdio.h>
#include "views/window.h"
#include "common_types.h"

/*draws last val from ring buffer*/

#define DECLARE_FUNC(type) \
    void drawFormat_##type(const void *settings); \
    drawFunc_p getDrawFunc(const type* var);

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

// void drawFormat_time_t(void *settings);

DECLARE_FUNC(Time_DateS)
DECLARE_FUNC(Time_HourS)


#undef DECLARE_FUNC
drawFunc_p getDrawFunc(const mtime_t* var);
void ValDrawTime(const void *settings);


#define TIMES_LABEL_LENGTH 8
drawFunc_p getDrawFunc(const TimeS* var);
void drawFormat_TimeS(const void *settings);


drawFunc_p getDrawFunc(const void* var);
void drawFormat_void(const void *settings);


drawFunc_p getDrawFunc(const char* var);
void drawFormat_char_p(const void *settings);

#define BAT_LEVEL_LABEL_LENGTH 5
drawFunc_p getDrawFunc(const Battery* var);
void draw_battery_level(const void *settings);


#endif
