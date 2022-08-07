#ifndef __display_H__
#define __display_H__

#include "screen.h"
#include "window.h"
#include "../../../tools/include/RingBuffer.h"
#include <stdbool.h>

// it must be declared before include of view
#include "views/view.h"

#include <types.h>

typedef struct Display
{
    View view;
    SensorData *data;
    uint8_t currentType;
} Display;

extern Display _Display;
typedef void (*view_new_func)(void);

void* getSettings(unsigned id);

void Display_init(SensorData *data);
void Display_setDisplayType(uint8_t type);
void Display_incDisplayType();
void Display_decDisplayType();
void Display_update();


#endif