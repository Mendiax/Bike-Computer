
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

View_Boot::View_Boot(const Sensor_Data& data, const Session_Data& session, gui::View_List* next)
    : gui::View(next), data{data}, session{session}
{

}

void View_Boot::render(void)
{
    auto creator = View_Creator::get_view();
    creator->reset();
    auto frame = creator->setup_bar(&this->data);

    auto frames = View_Creator::split_horizontal_arr(frame, 3);
    creator->add_value("rtc %f", 5, &data.boot.rtc, frames[0], Align::LEFT);
    creator->add_value("cfg %f", 5, &data.boot.config, frames[1], Align::LEFT);
    creator->add_value("sim %f", 5, &data.boot.sim868, frames[2], Align::LEFT);
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
