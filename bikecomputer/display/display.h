#ifndef __display_H__
#define __display_H__

#include "screen.h"
#include "screenfunc/plot.h"
#include "screenfunc/lastval.h"
#include "window.h"
#include "../sensors/data.h"
#include "screenfunc/label.h"
#include "screenfunc/val.h"

#define DISPLAYTYPELENGTH 3

#define MAX_NUMBER_OF_WINDOWS 2

#include "view.h"

enum DisplayType
{
    graph,
    lastVal,
    maxavg
};

uint8_t *mallocAllData()
{
    uint16_t maxSettingsSize = max(sizeof(LastValSettings), sizeof(PlotFloatSettings));
    maxSettingsSize = max(maxSettingsSize, sizeof(LabelSettings));
    maxSettingsSize = max(maxSettingsSize, sizeof(ValSettings));
    return malloc(sizeof(View) + maxSettingsSize * MAX_NUMBER_OF_WINDOWS);
}

typedef struct Display
{
    uint8_t *dataAlloc;
    SensorData *data;
    View *currentview;
    uint8_t currentType;
} Display;

Display _Display;

typedef void (*view_new_func)(void);

void view1_new(void)
{
    View *newView = (View *)_Display.dataAlloc;
    view_new_inAllocatedPlace(newView, 1);
    PlotFloatSettings *plotSettings = (PlotFloatSettings *)(_Display.dataAlloc + sizeof(View));
    *plotSettings = {0, 30};
    Window_new_inPlace(&newView->windows[0],
                       (Frame){0, 0, SCREEN_WIDTH, SCREEN_HEIGHT},
                       (void *)_Display.data->speedBuffer,
                       (void *)plotSettings,
                       PlotDoubleDraw);
}

void view2_new(void)
{
    View *newView = (View *)_Display.dataAlloc;
    view_new_inAllocatedPlace(newView, 2);

    LastValSettings *lastValSettings = (LastValSettings *)(_Display.dataAlloc + sizeof(View));
    *lastValSettings = (LastValSettings){
        .isInt = true,
        .format = "%2dkm/h",
        .maxLength = 6,
        .textSize = 3,
        .offsetX = 2,
        .offsetY = 8};
    Window_new_inPlace(&newView->windows[0],
                       (Frame){0, 0, SCREEN_WIDTH, SCREEN_HEIGHT},
                       (void *)_Display.data->speedBuffer,
                       (void *)lastValSettings,
                       LastValDraw);

    /*top header*/
    LabelSettings *headerSettings = (LabelSettings *)(_Display.dataAlloc + sizeof(View) + sizeof(LastValSettings));
    *headerSettings = (LabelSettings){
        .string = "speed",
        .textSize = 1,
        .offsetX = 2,
        .offsetY = 0};
    Window_new_inPlace(&newView->windows[1],
                       (Frame){0, 0, SCREEN_WIDTH, SCREEN_HEIGHT},
                       (void *)0,
                       (void *)headerSettings,
                       LabelDraw);
    return newView;
}

void view3_new(void)
{
    View *newView = (View *)_Display.dataAlloc;
    view_new_inAllocatedPlace(newView, 2);
    ValSettings *maxSpeedSettings = (ValSettings *)(_Display.dataAlloc + sizeof(View));
    *maxSpeedSettings = (ValSettings){
        .format = "%2d",
        .maxLength = 6,
        .textSize = 3,
        .offsetX = 2,
        .offsetY = 8};
    Window_new_inPlace(&newView->windows[0],
                       (Frame){0, 0, SCREEN_WIDTH, SCREEN_HEIGHT},
                       (void *)&(_Display.data->speedMax),
                       (void *)maxSpeedSettings,
                       ValDraw);

    /*top header*/
    LabelSettings *header2Settings = (LabelSettings *)(_Display.dataAlloc + sizeof(View) + sizeof(LastValSettings));
    *header2Settings = (LabelSettings){
        .string = "speed max",
        .textSize = 1,
        .offsetX = 2,
        .offsetY = 0};
    Window_new_inPlace(&newView->windows[1],
                       (Frame){0, 0, SCREEN_WIDTH, SCREEN_HEIGHT},
                       (void *)0,
                       (void *)header2Settings,
                       LabelDraw);
}

view_new_func views_all_new[] = {view1_new, view2_new, view3_new};

void Display_init(SensorData *data)
{
    _Display.data = data;
    _Display.currentType = 0;
    Screen_setup();

    _Display.dataAlloc = mallocAllData();
    view1_new();
    _Display.currentview = (View *)_Display.dataAlloc;
}

void Display_setDisplayType(DisplayType type)
{
    _Display.currentType = (uint8_t)type;
}

void Display_incDisplayType()
{
    _Display.currentType = (_Display.currentType + 1) % DISPLAYTYPELENGTH;
    assert(_Display.currentType >= 0 && _Display.currentType < DISPLAYTYPELENGTH);

    views_all_new[_Display.currentType]();
}

void Display_decDisplayType()
{
    _Display.currentType = _Display.currentType > 0 ? (_Display.currentType - 1) : DISPLAYTYPELENGTH - 1;
    assert(_Display.currentType >= 0 && _Display.currentType < DISPLAYTYPELENGTH);

    views_all_new[_Display.currentType]();
}

void Display_update()
{
    Screen_clear();

    //display data on screen
    view_draw(_Display.currentview);

    Screen_draw();
}

#endif