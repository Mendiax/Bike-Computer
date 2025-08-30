
// #------------------------------#
// |          includes            |
// #------------------------------#
// pico includes

// c/c++ includes
#include <tuple>
#include <inttypes.h>

// my includes
#include "gui/view_boot.hpp"
#include "views/view.hpp"
#include "traces.h"
#include "common_actors.hpp"
#include "gui/structure.hpp"

// #------------------------------#
// |           macros             |
// #------------------------------#

// #------------------------------#
// | global variables definitions |
// #------------------------------#

View_Boot::View_Boot(const SessionData& data, gui::View_List* next)
    : gui::View(next), data{data}
{

}

void View_Boot::render(void)
{
    auto creator = View_Creator::get_view();
    creator->reset();
    auto frame = creator->get_frame_bar();

    auto frames = View_Creator::split_horizontal_arr(frame, 3);
    creator->add_label("Booting", frames[0],Align::CENTER, 7);
    creator->add_value("time %4.1fs", 11, &data.sensors.boot.time, frames[1], Align::CENTER);
    creator->add_value("config %1.0f", 11, &data.sensors.boot.config, frames[2], Align::CENTER);
}

void View_Boot::action(void)
{
    PRINTF("View_Boot action\n");
    auto gui = Gui::get_gui();
    gui->enter();
}
// #------------------------------#
// | static variables definitions |
// #------------------------------#

// #------------------------------#
// | static functions declarations|
// #------------------------------#

// #------------------------------#
// | global function definitions  |
// #------------------------------#

// #------------------------------#
// | static functions definitions |
// #------------------------------#
