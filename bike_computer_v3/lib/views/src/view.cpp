#include <views/view.h>
#include "traces.h"
#include "massert.h"
#include <inttypes.h>

/*
    class setting up one view for display contaninig multiple windows
*/


void view_new_inAllocatedPlace(View *newView)
{
    newView->currnetNumberOfWindows = 0;
}

void view_draw(View *this_p)
{
    for (uint_fast8_t i = 0; i < this_p->currnetNumberOfWindows; i++)
    {
        TRACE_DEBUG(5, TRACE_VIEWS, "Drawing window %" PRIuFAST8 "\n", i);
        Window_update(&this_p->windows[i]);
        TRACE_DEBUG(6, TRACE_VIEWS, "window drawed %" PRIuFAST8 "\n", i);

    }
}

Window* view_getPreviousWindow(View *this_p)
{
    massert(this_p->currnetNumberOfWindows > 0, "cannot get previous window\n");
    return &this_p->windows[this_p->currnetNumberOfWindows-1]; 
}


void view_addNewWindow(View *this_p, const Window& window)
{
    TRACE_DEBUG(7, TRACE_VIEWS, "adding new window with id=%" PRIuFAST8 " \n", this_p->currnetNumberOfWindows);
    massert(this_p->currnetNumberOfWindows < MAX_NUMBER_OF_WINDOWS, "maximum numbers of windows reached in current view\n");
    this_p->windows[this_p->currnetNumberOfWindows] = window;
    this_p->currnetNumberOfWindows++;
}

