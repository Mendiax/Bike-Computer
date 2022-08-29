#ifndef __VIEW_H__
#define __VIEW_H__

#include "window.h"
#define MAX_NUMBER_OF_WINDOWS 9

#define VIEW_DEBUG

#ifdef VIEW_DEBUG
    #define VIEW_DEBUG_ADD(...) __VA_ARGS__
#else
    #define VIEW_DEBUG_ADD(...)
#endif
/*
    class setting up one view for display contaninig multiple windows
*/
typedef struct View
{
    uint8_t currnetNumberOfWindows;
    Window windows[MAX_NUMBER_OF_WINDOWS];
} View;

void view_new_inAllocatedPlace(View *newView);
void view_draw(View *this_p);

void view_addNewWindow(View *this_p, const Window& window);
Window* view_getPreviousWindow(View *this_p);

#endif