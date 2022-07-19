#ifndef __WINDOW_H__
#define __WINDOW_H__

#include <assert.h>
#include "frame.h"
#include <stdlib.h>

#ifdef __cplusplus
 extern "C" {
#endif

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

void Window_new_inPlace(Window *this_p, Frame newFrame, void *dataSource, void *drawSettings, drawFunc_p drawFunction);
Window *Window_new(Frame newFrame, void *dataSource, void *drawSettings, drawFunc_p drawFunction);
void Window_delete(Window **this_pp);
void Window_update(Window *this_p);

#ifdef __cplusplus
 }
#endif

#endif
