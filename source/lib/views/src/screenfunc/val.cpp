#include <views/screenfunc/val.h>
#include <display/fonts.h>
#include "views/frame.h"
#include <stdio.h>
#include <display/driver.hpp>
#include "views/screenfunc/common.h"
#include <stdint.h>

template<typename T>
void drawFormatVariable(const void *settings);

void drawFormatVariableVoid(const void *settings);


#define CREATE_FUNC(type) \
    void drawFormat_##type(const void *settings) \
    { \
        drawFormatVariable<type>(settings); \
    } \
    DrawFuncT getDrawFunc(const type* var) \
    { \
        (void) var; \
        return drawFormat_##type; \
    }


CREATE_FUNC(int8_t)
CREATE_FUNC(uint8_t)
CREATE_FUNC(uint16_t)
CREATE_FUNC(int16_t)
CREATE_FUNC(uint32_t)
CREATE_FUNC(int32_t)
CREATE_FUNC(uint64_t)
CREATE_FUNC(int64_t)
CREATE_FUNC(float)
CREATE_FUNC(double)

// void drawFormat_time_t(void *settings)
// {
//     ValDrawTime(settings);
// }
DrawFuncT getDrawFunc(const mtime_t* var)
{
    (void) var;
    return ValDrawTime;
}

DrawFuncT getDrawFunc(const TimeS* var)
{
    (void) var;
    return drawFormat_TimeS;
}


DrawFuncT getDrawFunc(const char* var)
{
    (void) var;
    return drawFormat_char_p;
}

DrawFuncT getDrawFunc(const Time_HourS* var)
{
    (void) var;
    return drawFormat_Time_HourS;
}

DrawFuncT getDrawFunc(const Time_DateS* var)
{
    (void) var;
    return drawFormat_Time_DateS;
}

DrawFuncT getDrawFunc(const Battery* var)
{
    (void) var;
    return draw_battery_level;
}

void drawFormat_void(const void *settings)
{
    drawFormatVariableVoid(settings);
}
DrawFuncT getDrawFunc(const void* var)
{
    (void) var;
    return drawFormat_void;
}

// void drawFormatInt8(void *settings)
// {
//     drawFormatVariable<int8_t>(settings);
// }
// void drawFormatInt16(void *settings)
// {
//     drawFormatVariable<int16_t>(settings);
// }
// void drawFormatInt32(void *settings)
// {
//     drawFormatVariable<int32_t>(settings);
// }
// void drawFormatInt64(void *settings)
// {
//     drawFormatVariable<int64_t>(settings);
// }
// void drawFormatFloat(void *settings)
// {
//     drawFormatVariable<float>(settings);
// }
// void drawFormatDouble(void *settings)
// {
//     drawFormatVariable<double>(settings);
// }


template<typename T>
void drawFormatVariable(const void *settings)
{
    ValSettings *valSettings = (ValSettings *)settings;

    T value = *((T *)valSettings->data);

    size_t max_str_len = valSettings->text.str_len + 1; // add 1 for '\0'
    char buffer[max_str_len];

    int write = snprintf(&buffer[0], max_str_len, valSettings->text.string, value);

    if(write < 0){
        return;
    }

    //PRINTF("[DRAW] %" PRIu16 ",%" PRIu16 "\n", valSettings->text.offsetX,valSettings->text.offsetY);

    display::println(valSettings->text.offsetX,valSettings->text.offsetY, buffer, valSettings->text.font, COLOR_WHITE, valSettings->text.scale);
}

void drawFormat_char_p(const void *settings)
{
    ValSettings *valSettings = (ValSettings *)settings;

    char* value = (char *)valSettings->data;

    size_t max_str_len = valSettings->text.str_len + 1;
    char buffer[max_str_len + 1];

    int write = snprintf(&buffer[0], max_str_len, valSettings->text.string, value);
    //PRINTF("value: '%s' max strlen: %zu format:'%s' buffer:'%s' scale:%zu font:%p\n", value, max_str_len, valSettings->text.string, &buffer[0], valSettings->text.scale, valSettings->text.font);

    if(write < 0){
        return;
    }

    display::println(valSettings->text.offsetX,valSettings->text.offsetY, buffer, valSettings->text.font, COLOR_WHITE, valSettings->text.scale);
}

void drawFormatVariableVoid(const void *settings)
{
    ValSettings *valSettings = (ValSettings *)settings;

    display::println(valSettings->text.offsetX,valSettings->text.offsetY, valSettings->text.string, valSettings->text.font, COLOR_WHITE, valSettings->text.scale);
}

void ValDrawTime(const void *settings)
{
    ValSettings *valSettings = (ValSettings *)settings;
    uint64_t time = *((uint64_t *)valSettings->data);

    const int hours = time / 3600;
    const int min = (time % 3600) / 60;
    const int sec = time % 60;


    unsigned max_str_len = valSettings->text.str_len + 1;
    char buffer[max_str_len + 1];

    int write = snprintf(&buffer[0], max_str_len, valSettings->text.string, hours, min, sec);

    if(write < 0){
        return;
    }

    display::println(valSettings->text.offsetX,valSettings->text.offsetY, buffer, valSettings->text.font, COLOR_WHITE, valSettings->text.scale);
}

void draw_battery_level(const void *settings)
{
    const ValSettings *valSettings = (ValSettings *)settings;

    const Battery& bat = *((Battery *)valSettings->data);

    const size_t max_str_len = 6;
    static char buffer[max_str_len + 1];
    static int_fast8_t last_bat;

    if(__builtin_expect(last_bat != bat.level, 0))
    {
        int write = -1;
        if(__builtin_expect(bat.is_charging, 0))
            write = snprintf(&buffer[0], max_str_len, "/\\%" PRIu8, bat.level);
        else
            write = snprintf(&buffer[0], max_str_len, "  %" PRIu8, bat.level);

        if(__builtin_expect(write < 0, 0)){
            return;
        }
        last_bat = bat.level;
    }

    display::println(valSettings->text.offsetX,valSettings->text.offsetY, buffer, valSettings->text.font, COLOR_WHITE, valSettings->text.scale);
}

void drawFormat_Time_DateS(const void *settings)
{
    const ValSettings *valSettings = (ValSettings *)settings;

    const Time_DateS& time = *((Time_DateS *)valSettings->data);
    const size_t max_str_len = valSettings->text.str_len + 1;
    char buffer[max_str_len];

    if(snprintf(&buffer[0], max_str_len, "%s%02" PRIu8 ".""%02" PRIu8 ".""%02" PRIu16, valSettings->text.string, time.day, time.month, time.year % 100 ) < 0){
        return;
    }
    display::println(valSettings->text.offsetX,valSettings->text.offsetY, buffer, valSettings->text.font, COLOR_WHITE, valSettings->text.scale);
}

void drawFormat_Time_HourS(const void *settings)
{
    const ValSettings *valSettings = (ValSettings *)settings;

    const Time_HourS& time = *((Time_HourS *)valSettings->data);
    const size_t max_str_len = valSettings->text.str_len + 1;
    char buffer[max_str_len];

    if(snprintf(&buffer[0], max_str_len,  "%s%02" PRIu8 ":""%02" PRIu8 ":""%02.0f",valSettings->text.string, time.hour, time.minutes, time.seconds) < 0){
        return;
    }
    display::println(valSettings->text.offsetX, valSettings->text.offsetY, buffer, valSettings->text.font, COLOR_WHITE, valSettings->text.scale);
}


void drawFormat_TimeS(const void *settings)
{
    const ValSettings *valSettings = (ValSettings *)settings;

    const TimeS& time = *((TimeS *)valSettings->data);
    const size_t max_str_len = valSettings->text.str_len + 1;
    char buffer[max_str_len];

    if(snprintf(&buffer[0], max_str_len, "%02" PRIu8 ".""%02" PRIu8 ".""%02" PRIu16, time.day, time.month, time.year ) < 0){
        return;
    }
    display::println(valSettings->text.offsetX,valSettings->text.offsetY, buffer, valSettings->text.font, COLOR_WHITE, valSettings->text.scale);

    const uint_fast16_t y_offset = valSettings->text.font->height * valSettings->text.scale;
    if(snprintf(&buffer[0], max_str_len,  "%02" PRIu8 ":""%02" PRIu8 ":""%02" PRIu8, time.hour, time.minutes, (uint8_t)time.seconds) < 0){
        return;
    }
    display::println(valSettings->text.offsetX, valSettings->text.offsetY + y_offset, buffer, valSettings->text.font, COLOR_WHITE, valSettings->text.scale);

    /*
    13.08.22
    15:32:12
    */
}