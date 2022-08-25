#include <assert.h>
#include "views/frame.h"
#include <stdlib.h>
#include "views/window.h"
#include "views/screenfunc/common.h"

// Window::Window()
// {

// }


void Window_new_inPlace(Window *this_p, const Settings& drawSettings, drawFunc_p drawFunction)
{
    this_p->settings = drawSettings;
    this_p->updateFunc_p = drawFunction;
}

void Window_update(Window *this_p)
{
    if (!this_p)
    {
        return;
    }
    //TRACE_FRAME_PRINT("Frame: " + String(this_p->frame.x) + "," + String(this_p->frame.y) + " update");
    this_p->updateFunc_p((void*)&this_p->settings);
}

