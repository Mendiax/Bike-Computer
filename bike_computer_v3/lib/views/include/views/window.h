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

    inline void update()
    {
        this->updateFunc_p((void*)&this->settings);
    }
};

// void Window_update(Window *this_p);

#endif
