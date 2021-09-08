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

enum DisplayType
{
    graph,
    lastVal,
    maxavg
};



/*
    class setting up one view for display contaninig multiple windows
*/
typedef struct View
{
    size_t numberOfWindows;
    Window* windows;
} View;

View *view_new(size_t numberOfWindows)
{
    View *newView = (View *)malloc(sizeof(View) + sizeof(Window) * numberOfWindows);
    newView->numberOfWindows = numberOfWindows;
    return newView;
}

void view_draw(View *this_p)
{
    for (size_t i = 0; i < this_p->numberOfWindows; i++)
    {
        Window_update(&this_p->windows[i]);
    }
}

void view_delete(View **this_pp)
{
    free(*this_pp);
    *this_pp = 0;
}

typedef struct Display
{
    uint8_t* dataAlloc;
    SensorData *data;
    View *currentview;
    uint8_t currentType;
} Display;

Display _Display;

typedef View *(*view_new_func)(void);
typedef void (*view_delete_func)(View **);

View *view1_new(void)
{
    View *newView = view_new(1);
    assert(newView);
    PlotFloatSettings *plotSettings = (PlotFloatSettings *)malloc(sizeof(PlotFloatSettings));
    *plotSettings = {0, 30};
    Window_new_inPlace(&newView->windows[0],
        (Frame){0, 0, SCREEN_WIDTH, SCREEN_HEIGHT},
        (void *)_Display.data->speedBuffer,
        (void *)plotSettings,
        PlotDoubleDraw);
    return newView;
}

void view1_delete(View **view1)
{
    //Window_delete(&(*view1)->windows[0]);
    view_delete(view1);
}

View *view2_new(void)
{
    View *newView = view_new(2);
    assert(newView);

    LastValSettings *lastValSettings = (LastValSettings *)malloc(sizeof(LastValSettings));
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
    LabelSettings *headerSettings = (LabelSettings *)malloc(sizeof(LabelSettings));
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

void view2_delete(View **view2)
{
    //Window_delete(&(*view2)->windows[0]);
    //Window_delete(&(*view2)->windows[1]);
    view_delete(view2);
}

View *view3_new(void)
{
    View *newView = view_new(2);
    assert(newView);
    LastValSettings *maxSpeedSettings = (LastValSettings *)malloc(sizeof(LastValSettings));
    *maxSpeedSettings = (LastValSettings){
        .isInt = true,
        .format = "%2d",
        .maxLength = 6,
        .textSize = 3,
        .offsetX = 2,
        .offsetY = 8};
    Window_new_inPlace(&newView->windows[0],
        (Frame){0, 0, SCREEN_WIDTH, SCREEN_HEIGHT},
        (void *)&_Display.data->speedMax,
        (void *)maxSpeedSettings,
        ValDraw);

    /*top header*/
    LabelSettings *header2Settings = (LabelSettings *)malloc(sizeof(LabelSettings));
    *header2Settings = (LabelSettings){
        .string = "speed",
        .textSize = 1,
        .offsetX = 2,
        .offsetY = 0};
    Window_new_inPlace(&newView->windows[1],
        (Frame){0, 0, SCREEN_WIDTH, SCREEN_HEIGHT},
        (void *)0,
        (void *)header2Settings,
        LabelDraw);
    return newView;
}

void view3_delete(View **view3)
{
    //Window_delete(&(*view3)->windows[0]);
    //Window_delete(&(*view3)->windows[1]);
    view_delete(view3);
}

view_new_func views_all_new[] = {view1_new, view2_new, view3_new};
view_delete_func views_all_delete[] = {view1_delete, view2_delete, view3_delete};

void Display_init(SensorData *data)
{
    _Display.data = data;
    _Display.currentType = 0;
    Screen_setup();

    _Display.currentview = views_all_new[_Display.currentType]();
}

//TODO add destroy to all created views

void Display_setDisplayType(DisplayType type)
{
    _Display.currentType = (uint8_t)type;
}

void Display_incDisplayType()
{
    views_all_delete[_Display.currentType](&_Display.currentview);
    _Display.currentType = (_Display.currentType + 1) % DISPLAYTYPELENGTH;
    assert(_Display.currentType >= 0 && _Display.currentType < DISPLAYTYPELENGTH);
    _Display.currentview = views_all_new[_Display.currentType]();
}

void Display_decDisplayType()
{
    views_all_delete[_Display.currentType](&_Display.currentview);
    _Display.currentType = _Display.currentType > 0 ? (_Display.currentType - 1) : DISPLAYTYPELENGTH - 1;
    assert(_Display.currentType >= 0 && _Display.currentType < DISPLAYTYPELENGTH);
    _Display.currentview = views_all_new[_Display.currentType]();
}

void Display_update()
{
    Screen_clear();

    //display data on screen
    view_draw(_Display.currentview);

    Screen_draw();
}

#endif