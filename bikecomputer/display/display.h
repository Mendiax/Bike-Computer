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
#define SETTING_SIZE 24//max(max(max(sizeof(LastValSettings), sizeof(PlotSettings)), sizeof(LabelSettings)), sizeof(ValSettings))

typedef struct setting{
    uint8_t setting[SETTING_SIZE];
} setting;

typedef struct Display
{
    View view;
    //uint8_t settingBuffer[SETTING_SIZE * MAX_NUMBER_OF_WINDOWS];
    setting settings[MAX_NUMBER_OF_WINDOWS];
    SensorData *data;
    View *currentview;
    uint8_t currentType;
} Display;

Display _Display;

typedef void (*view_new_func)(void);

void view1_new(void)
{
    View *newView = &_Display.view;
    view_new_inAllocatedPlace(newView, 1);
    PlotSettings *plotSettings = (PlotSettings *)(&_Display.settings[0]);
    *plotSettings = {0, 30};
    Window_new_inPlace(&newView->windows[0],
                       (Frame){0, 0, SCREEN_WIDTH, SCREEN_HEIGHT},
                       (void *)_Display.data->speedBuffer,
                       (void *)plotSettings,
                       PlotDrawByte);
}

void view2_new(void)
{
    View *newView = &_Display.view;
    view_new_inAllocatedPlace(newView, 3);

    LastValSettings *lastValSettings = (LastValSettings *)(&_Display.settings[0]);
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

    Window_new_inPlace(&newView->windows[0],
                       (Frame){SCREEN_WIDTH/2, 0, SCREEN_WIDTH/2, SCREEN_HEIGHT},
                       (void *)_Display.data->speedDistance,
                       (void *)lastValSettings,
                       ValDraw);

    /*top header*/
    LabelSettings *headerSettings = (LabelSettings *)(&_Display.settings[1]);
    *headerSettings = (LabelSettings){
        .string = "speed   distance",
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
    View *newView = &_Display.view;
    view_new_inAllocatedPlace(newView, 3);
    ValSettings *maxSpeedSettings = (ValSettings *)(&_Display.settings[0]);
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
    LabelSettings *header2Settings = (LabelSettings *)(&_Display.settings[1]);
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
    View *newView = &_Display.view;
    view_new_inAllocatedPlace(newView, 2);
    LastValSettings *topBarSettings = (LastValSettings *)(&_Display.settings[0]);
    *topBarSettings = (LastValSettings){
        .format = "rear shock: %2d",
        .maxLength = 15,
        .textSize = 1,
        .offsetX = 0,
        .offsetY = 0};
    Window_new_inPlace(&newView->windows[0],
                       (Frame){0, 0, SCREEN_WIDTH, SCREEN_HEIGHT_YELLOW},
                       (void *)(_Display.data->rearShockBuffer),
                       (void *)topBarSettings,
                       LastValDrawByte);

    
    PlotSettings *plotSettings = (PlotSettings *)(&_Display.settings[1]);
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
    _Display.currentType = 1;
    Screen_setup();

    //_Display.settings = &_Display.settingBuffer[0];
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
    view_draw(&_Display.view);

    Screen_draw();
}

#endif