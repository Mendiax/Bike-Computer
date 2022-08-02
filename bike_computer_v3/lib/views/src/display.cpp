#include <views/screen.h>
#include <views/window.h>

#include <algorithm>
#include "views/display.h"


#include <views/screenfunc/plot.h>
#include <views/screenfunc/lastval.h>
#include <views/screenfunc/label.h>
#include <views/screenfunc/val.h>

Display _Display;

typedef void (*view_new_func)(void);

#define WINDOW_CREATE_LASTVAL(originX,originY, maxWidth, maxHeight) \
do { \
\
} while (0)

// views
extern void view0(void);
extern void view1(void);
extern void view2(void);
extern void view3(void);

extern uint8_t allocData[];

//view_new_func views_all[] = {view0, view1, view2, view3, view4};
view_new_func views_all[] = {view0, view1};
#define DISPLAY_TYPES_LENGTH (sizeof(views_all) / sizeof(view_new_func))
void Display_init(SensorData *data)
{
    _Display.data = data;
    _Display.currentType = 0;
    
    // setup
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

    //Screen_draw();
}
