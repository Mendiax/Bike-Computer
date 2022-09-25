#ifndef __WINDOW_H__
#define __WINDOW_H__

#include <assert.h>
#include "frame.h"
#include <stdlib.h>
#include "views/screenfunc/common.h"

/* class containg data to draw on window */
struct Window
{
    Settings settings;
    drawFunc_p updateFunc_p;

};

void Window_new_inPlace(Window *this_p, const Settings& drawSettings, drawFunc_p drawFunction);

void Window_update(Window *this_p);

#endif
