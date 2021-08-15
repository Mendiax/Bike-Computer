#ifndef __display_H__
#define __display_H__

#include "screen.h"
#include "screenfunc/plot.h"
#include "window.h"
#include "../sensors/data.h"

enum Display_type
{
    graph,
    lastVal
};

/*
    class setting up one view for display contaninig multiple windows
*/

typedef struct View
{
    size_t numberOfWindows;
    Window *windows[];
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
        (*this_p->windows[i]).update();
    }
}

void view_delete(View **this_pp)
{
    free(*this_pp);
    *this_pp = 0;
}

typedef struct Display
{
    SensorData *data;
    View *views[1];
    Display_type currentType;
} Display;

Display _Display;

void Display_init(SensorData *data)
{
    _Display.data = data;
    _Display.currentType = (Display_type)0;
    Screen_setup();

    /* 2 frames */
    _Display.views[0] = view_new(2);
    PlotDoubleSettings* plotSettings = (PlotDoubleSettings*)malloc(sizeof(PlotDoubleSettings));
    *plotSettings = {0, 30};
    (*_Display.views[0]).windows[0] = new Window(
        (Frame){0, 0, SCREEN_WIDTH/2, SCREEN_HEIGHT},
        (void *)_Display.data->velocity,
        (void *)plotSettings,
        PlotDoubleDraw);
    (*_Display.views[0]).windows[1] = new Window(
        (Frame){SCREEN_WIDTH/2, 0, SCREEN_WIDTH/2, SCREEN_HEIGHT},
        (void *)_Display.data->velocity,
        (void *)plotSettings,
        PlotDoubleDraw);
}

//TODO add destroy to all created views

void Display_setDisplayType(Display_type type)
{
    _Display.currentType = type;
}

void Display_update()
{
    Screen_clear();
    //display data on screen
    view_draw(_Display.views[_Display.currentType]);
    Screen_draw();
}

#endif