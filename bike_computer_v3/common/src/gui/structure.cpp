
// #------------------------------#
// |          includes            |
// #------------------------------#
// pico includes

// c/c++ includes

// my includes
#include "views/view.hpp"

#include "gui/structure.hpp"
#include "gui_common.hpp"
#include "gui/view_all.hpp"
#include "display/print.h"
#include "massert.h"

// #------------------------------#
// |           macros             |
// #------------------------------#

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


// gui
Gui* Gui::singleton = nullptr;


Gui::Gui(Sensor_Data* data_p, Session_Data* session_p)
{
    display::init();
    display::clear();
    display::display();

    this->set_data(data_p, session_p);
    this->create();
    this->render();
}

Gui::~Gui()
{

}
void Gui::set_data(Sensor_Data* data_p, Session_Data* session_p)
{
    data = data_p;
    session = session_p;
}
void Gui::create()
{
    auto view1 = new View_Velocity(*data, *session, false);

    auto session_menu = new View_List();
    session_menu->add_view(view1);
    session_menu->add_view(new View_Max_Avg(*data, *session, false));
    session_menu->add_view(new View_Gps(*data, *session, false));
    session_menu->add_view(new View_Date(*data, *session, false));
    // session_menu->add_view(new View_Forecast(*data, *session, false));
    session_menu->add_view(new View_Total(*data, *session, false));



    this->current_view_list = session_menu;
    massert(view1 == get_current(), "current view on create failed\n");
}


void Gui::handle_buttons()
{
    #define BTN_HANDLE(btn, short_clb, long_clb) \
        do { \
            if(btn.pop_was_pressed()) \
            { \
                short_clb; \
            } \
            if(btn.pop_was_pressed_long()) \
            { \
                long_clb; \
            } \
        } while(0)

    BTN_HANDLE(BTN_NAVIGATE,
               this->go_next(),
               this->go_prev());

    BTN_HANDLE(BTN_ACTION,
               this->get_current()->action(),
               this->get_current()->action_long());

    // if(BTN_NAVIGATE.pop_was_pressed())
    // {
    //     this->go_next();
    // }
    // if(BTN_NAVIGATE.pop_was_pressed_long())
    // {
    //     this->go_prev();
    //     // this->current_view_list = this->current_view_list->get_back();
    // }

    // // enter
    // if(BTN_ACTION.pop_was_pressed())
    // {
    //     this->get_current()->action();
    // }
    // if(BTN_ACTION.pop_was_pressed_long())
    // {
    //     this->get_current()->action_long();
    // }
    #undef BTN_HANDLE
}


void Gui::go_next()
{
    current_view_list->get_next_view();
    this->render();
}
void Gui::go_prev()
{
    current_view_list->get_prev_view();
    this->render();
}
void Gui::go_back()
{
    auto top = current_view_list->get_back();
    if(top != nullptr)
        current_view_list = top;
    this->render();
}
void Gui::enter()
{
    auto top = (View_List*)current_view_list->get_current_view()->get_next_view_list();
    if(top != nullptr)
        current_view_list = top;
    this->render();
}
View* Gui::get_current()
{
    return current_view_list->get_current_view();
}

void Gui::refresh()
{
    display::clear();
    View_Creator::get_view()->draw();
    display::display();
}

void Gui::render()
{
    display::clear();
    current_view_list->get_current_view()->render();
    this->refresh();
}

// #------------------------------#
// | static functions definitions |
// #------------------------------#
