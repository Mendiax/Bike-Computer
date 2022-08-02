#include <views/screenfunc/val.h>
#include <display/fonts.h>
#include "views/frame.h"
#include <stdio.h>
#include <display/print.h>
#include "views/screenfunc/common.h"

template<typename T>
void drawFormatVariable(void *settings);

#define CREATE_FUNC(type) \
    void drawFormat_##type(void *settings) \
    { \
        drawFormatVariable<type>(settings); \
    } \
    drawFunc_p getDrawFunc(type* var) \
    { \
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

void drawFormatVariableVoid(void *settings);

void drawFormat_void(void *settings)
{ 
    drawFormatVariableVoid(settings); 
} 
drawFunc_p getDrawFunc(void* var) 
{ 
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
void drawFormatVariable(void *settings)
{
    ValSettings *valSettings = (ValSettings *)settings;

    T value = *((T *)valSettings->data);

    size_t maxStrLen = valSettings->text.str_len + 1;
    char buffer[maxStrLen + 1];

    int write = snprintf(&buffer[0], maxStrLen, valSettings->text.string, value);

    if(write < 0){
        return;
    }

    Paint_Println(valSettings->text.offsetX,valSettings->text.offsetY, buffer, valSettings->text.font, COLOR_WHITE, valSettings->text.scale);
}
void drawFormatVariableVoid(void *settings)
{
    ValSettings *valSettings = (ValSettings *)settings;

    Paint_Println(valSettings->text.offsetX,valSettings->text.offsetY, valSettings->text.string, valSettings->text.font, COLOR_WHITE, valSettings->text.scale);
}

void ValDrawTime(void *data, void *settings, Frame *plotFrame)
{
    ValSettings *valSettings = (ValSettings *)settings;
    uint32_t time = *((uint32_t *)data);

    char hours = time / 3600;
    char min = (time % 3600) / 60;
    char sec = time % 60; 


    unsigned maxStrLen = valSettings->text.str_len + 1;
    char buffer[maxStrLen + 1];

    int write = snprintf(&buffer[0], maxStrLen, valSettings->text.string, hours, min, sec);

    if(write < 0){
        return;
    }

    int x = plotFrame->x + valSettings->text.offsetX;
    int y = plotFrame->y +valSettings->text.offsetY;
    Paint_Println(x, y, buffer, valSettings->text.font, COLOR_WHITE, valSettings->text.scale);
}
