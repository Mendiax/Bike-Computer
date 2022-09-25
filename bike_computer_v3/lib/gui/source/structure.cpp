
// #------------------------------#
// |          includes            |
// #------------------------------#
// pico includes

// c/c++ includes

// my includes
#include "views/view.hpp"
#include "structure.hpp"
#include "gui_common.hpp"
#include "view_velocity.hpp"
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

static void on_click_view_action();
static void on_click_view_action()
{
    auto gui = gui::Gui::get_gui();
    // gui->get_current()->action();
}


static void on_click_next_view();
static void on_click_next_view()
{
    auto gui = gui::Gui::get_gui();
    gui->go_next();
}



Gui::Gui(Sensor_Data* data_p, Session_Data* session_p)
{
    BTN_NAVIGATE.set_callback(on_click_next_view);
    display::init();
    display::clear();
    display::display();
    PRINTF("Gui create\n");

    this->set_data(data_p, session_p);
    this->create();
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

    this->current_view_list = session_menu;
    massert(view1 == get_current(), "current view on create failed\n");
}


void Gui::go_next()
{
    current_view_list->get_next_view();
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
}

// #------------------------------#
// | static functions definitions |
// #------------------------------#
