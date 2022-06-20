#ifndef __display_H__
#define __display_H__

#include "screen.h"
#include "screenfunc/plot.h"
#include "screenfunc/lastval.h"
#include "window.h"
#include "../sensors/data.h"
#include "screenfunc/label.h"
#include "screenfunc/val.h"

#define MAX_NUMBER_OF_WINDOWS 3

#include "view.h"

#define SETTING_SIZE max(max(max(sizeof(LastValSettings), sizeof(PlotSettings)), sizeof(LabelSettings)), sizeof(ValSettings))
uint8_t allocData[SETTING_SIZE * MAX_NUMBER_OF_WINDOWS + sizeof(View)];

void* getSettings(unsigned id){
    assert(id < MAX_NUMBER_OF_WINDOWS);
    return allocData + sizeof(View) + id * SETTING_SIZE;
}

typedef struct Display
{
    const uint8_t *dataAlloc;
    SensorData *data;
    uint8_t currentType;
} Display;

Display _Display;

typedef void (*view_new_func)(void);



/*
    speed, distance 
*/
void view0(void)
{
    View *newView = (View *)_Display.dataAlloc;
    view_new_inAllocatedPlace(newView, 3);

    LastValSettings *lastValSettings = (LastValSettings *)getSettings(0);
    *lastValSettings = (LastValSettings){
        .format = "%2d",
        .maxLength = 2,
        .textSize = 3,
        .offsetX = 2,
        .offsetY = 8};
    Window_new_inPlace(&newView->windows[0],
                       (Frame){0, 0, SCREEN_WIDTH/2, SCREEN_HEIGHT},
                       (void *)_Display.data->speedBuffer,
                       (void *)lastValSettings,
                       LastValDrawByte);
    
    Window_new_inPlace(&newView->windows[1],
                       (Frame){SCREEN_WIDTH/2, 0, SCREEN_WIDTH/2, SCREEN_HEIGHT},
                       (void *)&(_Display.data->speedDistance),
                       (void *)lastValSettings,
                       ValDrawInt);

    /*top header*/
    LabelSettings *headerSettings = (LabelSettings *)getSettings(1);
    *headerSettings = (LabelSettings){
        .string = "speed     distance",
        .textSize = 1,
        .offsetX = 2,
        .offsetY = 0};
    Window_new_inPlace(&newView->windows[2],
                       (Frame){0, 0, SCREEN_WIDTH, SCREEN_HEIGHT},
                       (void *)0,
                       (void *)headerSettings,
                       LabelDraw);
}


/* 
    max, avg display
*/
void view1(void)
{
    View *newView = (View *)_Display.dataAlloc;
    view_new_inAllocatedPlace(newView, 3);
    ValSettings *maxSpeedSettings = (ValSettings *)getSettings(0);
    *maxSpeedSettings = (ValSettings){
        .format = "%2d",
        .maxLength = 6,
        .textSize = 3,
        .offsetX = 2,
        .offsetY = 8};
    Window_new_inPlace(&newView->windows[0],
                       (Frame){0, 0, SCREEN_WIDTH/2, SCREEN_HEIGHT},
                       (void *)&(_Display.data->speedMax),
                       (void *)maxSpeedSettings,
                       ValDraw);
    Window_new_inPlace(&newView->windows[1],
                       (Frame){SCREEN_WIDTH/2, 0, SCREEN_WIDTH/2, SCREEN_HEIGHT},
                       (void *)&(_Display.data->speedAvgVal),
                       (void *)maxSpeedSettings,
                       ValDraw);

    /*top header*/
    LabelSettings *header2Settings = (LabelSettings *)getSettings(1);
    *header2Settings = (LabelSettings){
        .string = " max        avg",
        .textSize = 1,
        .offsetX = 2,
        .offsetY = 0};
    Window_new_inPlace(&newView->windows[2],
                       (Frame){0, 0, SCREEN_WIDTH, SCREEN_HEIGHT},
                       (void *)0,
                       (void *)header2Settings,
                       LabelDraw);
}


/*
    speed plot
*/
void view2(void)
{
    View *newView = (View *)_Display.dataAlloc;
    view_new_inAllocatedPlace(newView, 1);
    PlotSettings *plotSettings = (PlotSettings *)getSettings(0);
    *plotSettings = {
        .min = 0,
        .max = 30,
        .autoMax = true,
        .autoMin = false,
        .offset = 5};
    Window_new_inPlace(&newView->windows[0],
                       (Frame){0, 0, SCREEN_WIDTH, SCREEN_HEIGHT},
                       (void *)_Display.data->speedBuffer,
                       (void *)plotSettings,
                       PlotDrawByte);
}


/*
    rear shock plot
*/
void view3(void)
{
    View *newView = (View *)_Display.dataAlloc;
    view_new_inAllocatedPlace(newView, 2);
    LastValSettings *topBarSettings = (LastValSettings *)getSettings(0);
    *topBarSettings = (LastValSettings){
        .format = "rear shock: %2d",
        .maxLength = 15,
        .textSize = 1,
        .offsetX = 0,
        .offsetY = 0};
    Window_new_inPlace(&newView->windows[0],
                       (Frame){0, 0, SCREEN_WIDTH, SCREEN_HEIGHT_YELLOW},
                       (void *)_Display.data->rearShockBuffer,
                       (void *)topBarSettings,
                       LastValDrawByte);

    
    PlotSettings *plotSettings = (PlotSettings *)getSettings(1);
    *plotSettings = {
        .min = 0,
        .max = 100,
        .autoMax = true,
        .autoMin = true,
        .offset = 5};
    Window_new_inPlace(&newView->windows[1],
                       (Frame){0, SCREEN_HEIGHT_YELLOW, SCREEN_WIDTH, SCREEN_HEIGHT_BLUE},
                       (void *)_Display.data->rearShockBuffer,
                       (void *)plotSettings,
                       PlotDrawByte);
}

void view4(void)
{
    View *newView = (View *)_Display.dataAlloc;
    view_new_inAllocatedPlace(newView, 1);
    ValSettings *topBarSettings = (ValSettings *)getSettings(0);
    *topBarSettings = (ValSettings){
        .format = "%2d:%2d:%2d",
        .maxLength = 15,
        .textSize = 3,
        .offsetX = 0,
        .offsetY = 10};
    Window_new_inPlace(&newView->windows[0],
                       (Frame){0, 0, SCREEN_WIDTH, SCREEN_HEIGHT_YELLOW},
                       (void *)&(_Display.data->time),
                       (void *)topBarSettings,
                       ValDrawTime);
}

view_new_func views_all[] = {view0, view1, view2, view3, view4};
byte DISPLAY_TYPES_LENGTH = sizeof(views_all) / sizeof(view_new_func);
void Display_init(SensorData *data)
{
    _Display.data = data;
    _Display.currentType = 0;
    Screen_setup();

    _Display.dataAlloc = allocData;
    views_all[_Display.currentType]();
}

void Display_setDisplayType(uint8_t type)
{
    _Display.currentType = (uint8_t)type;
}

void Display_incDisplayType()
{
    _Display.currentType = (_Display.currentType + 1) % DISPLAY_TYPES_LENGTH;
    assert(_Display.currentType >= 0 && _Display.currentType < DISPLAY_TYPES_LENGTH);

    views_all[_Display.currentType]();
}

void Display_decDisplayType()
{
    _Display.currentType = _Display.currentType > 0 ? (_Display.currentType - 1) : DISPLAY_TYPES_LENGTH - 1;
    assert(_Display.currentType >= 0 && _Display.currentType < DISPLAY_TYPES_LENGTH);

    views_all[_Display.currentType]();
}

void Display_update()
{
    Screen_clear();

    //display data on screen
    view_draw((View*)_Display.dataAlloc);

    Screen_draw();
}

#endif