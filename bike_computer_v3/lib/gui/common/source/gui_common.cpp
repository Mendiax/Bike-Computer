
// #------------------------------#
// |          includes            |
// #------------------------------#
// pico includes

// c/c++ includes

// my includes
#include "gui_common.hpp"
#include "buttons/buttons.h"
#include "traces.h"

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

View::View(view_list_p next)
{
    this->next = next;
}
View::~View()
{

}

view_list_p View::get_next_view_list()
{
    return next;
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
