
// #------------------------------#
// |          includes            |
// #------------------------------#
// pico includes

// c/c++ includes

// my includes
#include "gui_common.hpp"
#include "buttons/buttons.h"
#include "traces.h"
#include "views/view.hpp"

// #------------------------------#
// |           macros             |
// #------------------------------#


// Button btn0;
// Button btn1;
// Button btn2;
// Button btn3;

// #------------------------------#
// | global variables definitions |
// #------------------------------#

// #------------------------------#
// | static variables definitions |
// #------------------------------#

// #------------------------------#
// | static functions declarations|
// #------------------------------#

// #------------------------------#
// | global function definitions  |
// #------------------------------#

using namespace gui;

View::View(View_List* next)
{
    this->next = next;
}
View::~View()
{

}

void View::refresh(void)
{
    View_Creator::get_view()->draw();
}

View_List* View::get_next_view_list()
{
    return next;
}

void View::action(void)
{
    // empty handler
}
void View::action_long(void)
{
    // empty handler
}

void View::action_second(void)
{

}
void View::action_second_long(void)
{

}




View_List::View_List()
{
    it = views_list.begin();
    back = nullptr;
}

View_List::View_List(View_List* top)
{
    it = views_list.begin();
    back = top;
}

View_List::~View_List()
{

}

View* View_List::get_next_view()
{
    it++;
    it = it == views_list.end() ? it = views_list.begin() : it;
    return *it;
}

View* View_List::get_prev_view()
{
    it = it == views_list.begin() ? it = views_list.end() : it;
    it--;
    return *it;
}

View* View_List::get_current_view()
{
    return *it;
}

View_List* View_List::get_back()
{
    return back;
}


void View_List::add_view(View* new_view)
{
    views_list.push_back(new_view);
    it = views_list.begin();
}






// #------------------------------#
// | static functions definitions |
// #------------------------------#
