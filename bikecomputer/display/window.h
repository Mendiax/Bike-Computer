#ifndef __WINDOW_H__
#define __WINDOW_H__

#include "../addons/print.h"
#include <assert.h>
#include "screen.h"

/* data ptr, additional data ptr, window(size) */
typedef void (*drawFunc_p)(void *, void *, Frame *);

/* class containg data to draw on window */
typedef struct window
{
    Frame frame;
    void *data;
    void *settings;
    drawFunc_p updateFunc_p;
} Window;

void Window_new_inPlace(Window *this_p, Frame newFrame, void *dataSource, void *drawSettings, drawFunc_p drawFunction)
{
    this_p->frame = newFrame;
    this_p->data = dataSource;
    this_p->settings = drawSettings;
    this_p->updateFunc_p = drawFunction;

    assert(this_p->frame.x >= 0 && this_p->frame.x <= SCREEN_WIDTH);
    assert(this_p->frame.width >= 0 && this_p->frame.width <= SCREEN_WIDTH);
    assert(this_p->frame.y >= 0 && this_p->frame.y <= SCREEN_HEIGHT);
    assert(this_p->frame.height >= 0 && this_p->frame.height <= SCREEN_HEIGHT);
}

Window *Window_new(Frame newFrame, void *dataSource, void *drawSettings, drawFunc_p drawFunction)
{
    Window *this_p = (Window *)malloc(sizeof(Window));
    Window_new_inPlace(this_p, newFrame, dataSource, drawSettings, drawFunction);
    return this_p;
}

void Window_delete(Window **this_pp)
{
    if ((*this_pp)->settings)
    {
        free((*this_pp)->settings);
    }
    (*this_pp)->settings = (void *)0;
    free(*this_pp);
    *this_pp = (Window *)0;
}

void Window_update(Window *this_p)
{
    if (!this_p)
    {
        return;
    }
    //TRACE_FRAME_PRINT("Frame: " + String(this_p->frame.x) + "," + String(this_p->frame.y) + " update");
    this_p->updateFunc_p(this_p->data, this_p->settings, &this_p->frame);
}

#endif
