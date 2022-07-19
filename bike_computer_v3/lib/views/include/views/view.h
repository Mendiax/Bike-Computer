#ifndef __VIEW_H__
#define __VIEW_H__

#include "window.h"
#define MAX_NUMBER_OF_WINDOWS 8

/*
    class setting up one view for display contaninig multiple windows
*/
typedef struct View
{
    size_t numberOfWindows;
    Window windows[MAX_NUMBER_OF_WINDOWS];
} View;

View *view_new(size_t numberOfWindows);
void view_new_inAllocatedPlace(View *newView ,size_t numberOfWindows);
void view_draw(View *this_p);
void view_delete(View **this_pp);

#endif