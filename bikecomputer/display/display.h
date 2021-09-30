#ifndef __display_H__
#define __display_H__

#include "screen.h"
#include "screenfunc/plot.h"
#include "screenfunc/lastval.h"
#include "window.h"
#include "../sensors/data.h"
#include "screenfunc/label.h"
#include "screenfunc/val.h"

#define DISPLAYTYPELENGTH 4

#define MAX_NUMBER_OF_WINDOWS 3

#include "view.h"

enum DisplayType
{
    graph,
    lastVal,
    maxavg
};

uint8_t *mallocAllData()
{
    uint16_t maxSettingsSize = max(sizeof(LastValSettings), sizeof(PlotSettings));
    maxSettingsSize = max(maxSettingsSize, sizeof(LabelSettings));
    maxSettingsSize = max(maxSettingsSize, sizeof(ValSettings));
    return (uint8_t *)malloc(sizeof(View) + maxSettingsSize * MAX_NUMBER_OF_WINDOWS);
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
    PlotSettings *plotSettings = (PlotSettings *)(_Display.dataAlloc + sizeof(View));
    *plotSettings = {0, 30};
    Window_new_inPlace(&newView->windows[0],
                       (Frame){0, 0, SCREEN_WIDTH, SCREEN_HEIGHT},
                       (void *)_Display.data->speedBuffer,
                       (void *)plotSettings,
                       PlotDrawByte);
}

void view2_new(void)
{
    View *newView = (View *)_Display.dataAlloc;
    view_new_inAllocatedPlace(newView, 2);

    LastValSettings *lastValSettings = (LastValSettings *)(_Display.dataAlloc + sizeof(View));
    *lastValSettings = (LastValSettings){
        .format = "%2dkm/h",
        .maxLength = 6,
        .textSize = 3,
        .offsetX = 2,
        .offsetY = 8};
    Window_new_inPlace(&newView->windows[0],
                       (Frame){0, 0, SCREEN_WIDTH, SCREEN_HEIGHT},
                       (void *)_Display.data->speedBuffer,
                       (void *)lastValSettings,
                       LastValDrawByte);

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
}

void view3_new(void)
{
    View *newView = (View *)_Display.dataAlloc;
    view_new_inAllocatedPlace(newView, 3);
    ValSettings *maxSpeedSettings = (ValSettings *)(_Display.dataAlloc + sizeof(View));
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
    LabelSettings *header2Settings = (LabelSettings *)(_Display.dataAlloc + sizeof(View) + sizeof(LastValSettings));
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
/*rear shock plot*/
void view4_new(void)
{
    View *newView = (View *)_Display.dataAlloc;
    view_new_inAllocatedPlace(newView, 2);
    LastValSettings *topBarSettings = (LastValSettings *)(_Display.dataAlloc + sizeof(View));
    *topBarSettings = (LastValSettings){
        .format = "rear shock: %2d",
        .maxLength = 15,
        .textSize = 1,
        .offsetX = 0,
        .offsetY = 0};
    Window_new_inPlace(&newView->windows[0],
                       (Frame){0, 0, SCREEN_WIDTH, SCREEN_HEIGHT_YELLOW},
                       (void *)&(_Display.data->rearShockBuffer),
                       (void *)topBarSettings,
                       LastValDrawByte);

    
    PlotSettings *plotSettings = (PlotSettings *)(_Display.dataAlloc + sizeof(View) + sizeof(LastValSettings));
    *plotSettings = {0, 100};
    Window_new_inPlace(&newView->windows[1],
                       (Frame){0, SCREEN_HEIGHT_YELLOW, SCREEN_WIDTH, SCREEN_HEIGHT_BLUE},
                       (void *)_Display.data->rearShockBuffer,
                       (void *)plotSettings,
                       PlotDrawByte);
}

view_new_func views_all_new[] = {view1_new, view2_new, view3_new,view4_new};

void Display_init(SensorData *data)
{
    _Display.data = data;
    _Display.currentType = 2;
    Screen_setup();

    _Display.dataAlloc = mallocAllData();
    _Display.currentview = (View *)_Display.dataAlloc;
    views_all_new[_Display.currentType]();
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