
// #------------------------------#
// |          includes            |
// #------------------------------#
// pico includes

// c/c++ includes

// my includes
#include "gui/main/main_new_session.hpp"
#include "gui/structure.hpp"
#include "views/view.hpp"
#include "traces.h"


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

void Main_New_Session::render()
{
    auto creator = View_Creator::get_view();
    creator->reset();
    auto gui = Gui::get_gui();
    auto frame = creator->setup_bar(&gui->data->current_time.hours, &gui->data->lipo);
    auto [top, bot] = View_Creator::split_horizontal(frame);

    creator->add_label("New", top, Align::CENTER, 7);
    auto window = creator->get_previous_window();
    labelSettingsAlignHeight(window->settings.label.text, top, false);

    creator->add_label("session", bot, Align::CENTER, 7);

}

void Main_New_Session::action()
{
    // PRINTF("main manu new session btn pressed next list:%p\n", this->get_next_view_list());
    auto gui = Gui::get_gui();
    gui->enter();
}

// #------------------------------#
// | static functions definitions |
// #------------------------------#
