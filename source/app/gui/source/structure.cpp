
// #------------------------------#
// |          includes            |
// #------------------------------#
// pico includes

// c/c++ includes

// my includes
#include "gui/view_last_avg.hpp"
#include "gui/view_last_date.hpp"
#include "gui/view_last_time.hpp"
#include "views/view.hpp"

#include "gui/structure.hpp"
#include "gui_common.hpp"
#include "gui/view_all.hpp"
#include "display/driver.hpp"
#include "massert.hpp"

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
    PRINTF("Gui::Gui()\n");


    this->set_data(data_p, session_p);
    this->create();
    // this->render();
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
    auto main_menu = new View_List();
    // -----------------------------
    //      session
    // -----------------------------
    auto session_menu = new View_List(main_menu);
    session_menu->add_view(new View_Velocity(*data, *session));
    session_menu->add_view(new View_Max_Avg(*data, *session));
    session_menu->add_view(new View_Gps(*data, *session));
    session_menu->add_view(new View_Date(*data, *session));
    session_menu->add_view(new View_Total(*data, *session));
    // -----------------------------
    //      history
    // -----------------------------
    auto history_session_menu = new View_List(main_menu);
    history_session_menu->add_view(new View_Last_Time(*data, *session));
    history_session_menu->add_view(new View_Last_Avg(*data, *session));
    history_session_menu->add_view(new View_Last_Date(*data, *session));
    // -----------------------------
    //      main menu
    // -----------------------------
    main_menu->add_view(new Main_New_Session((gui::View_List*)session_menu));
    main_menu->add_view(new Main_History((gui::View_List*)history_session_menu));

    this->current_view_list = main_menu;
}

static void blank(){}

void Gui::handle_buttons()
{
    #define BTN_HANDLE(btn, short_clb, long_clb) \
        do { \
            if(btn.pop_was_pressed()) \
            { \
                short_clb; \
            } \
            if(btn.is_pressed_long_execute() || btn.pop_was_pressed_long()) \
            { \
                long_clb; \
            } \
        } while(0)

    BTN_HANDLE(BTN_NAVIGATE,
               this->go_next(),
               blank());

    BTN_HANDLE(BTN_NAVIGATE_BACK,
               this->go_prev(),
               blank());



    BTN_HANDLE(BTN_ACTION,
               this->get_current()->action(),
               this->get_current()->action_long());

    BTN_HANDLE(BTN_ACTION_SECOND,
               this->get_current()->action_second(),
               this->get_current()->action_second_long());

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
    // display::clear();
    // View_Creator::get_view()->draw();
    current_view_list->get_current_view()->refresh();
}

void Gui::render()
{
    // display::clear();
    current_view_list->get_current_view()->render();
    this->refresh();
}

// #------------------------------#
// | static functions definitions |
// #------------------------------#
