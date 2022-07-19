#ifndef __display_H__
#define __display_H__

#include "screen.h"
#include "window.h"
#include "../../../tools/include/RingBuffer.h"
#include <stdbool.h>

// it must be declared before include of view
#include "views/view.h"

typedef struct SensorData
{
    RingBuffer* rearShockBuffer;
    RingBuffer* speedBuffer;
    float speedMax;
    float speedAvgVal;
    unsigned long speedAvgCnt;
    unsigned speedDistance;
    unsigned speedDistanceHundreth;
    unsigned long time;
    bool lipoCharging;
    int lipoLevel; // in %
} SensorData;

typedef struct Display
{
    const uint8_t *dataAlloc;
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