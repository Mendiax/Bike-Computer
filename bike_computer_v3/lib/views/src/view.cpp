#include <views/view.h>

/*
    class setting up one view for display contaninig multiple windows
*/


View *view_new(size_t numberOfWindows)
{
    View *newView = (View *)malloc(sizeof(View) + sizeof(Window) * numberOfWindows);
    newView->numberOfWindows = numberOfWindows;
    return newView;
}

void view_new_inAllocatedPlace(View *newView ,size_t numberOfWindows)
{
    newView->numberOfWindows = numberOfWindows;
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
