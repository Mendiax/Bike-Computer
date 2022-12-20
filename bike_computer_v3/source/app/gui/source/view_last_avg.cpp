
// #------------------------------#
// |          includes            |
// #------------------------------#
// pico includes

// c/c++ includes
#include <tuple>

// my includes
#include "common_types.h"
#include "gui/view_last_time.hpp"
#include "views/view.hpp"
#include "gui/view_last_avg.hpp"

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
void View_Last_Avg::render(void)
{
    auto creator = View_Creator::get_view();
    creator->reset();
    auto frame = creator->setup_bar(&this->data.current_time.hours, &this->data.lipo);

    // frame.height = DISPLAY_HEIGHT;

    auto frames = View_Creator::split_horizontal_arr(frame, 2);
    creator->add_value("max  :%4.2f", 10, &this->session.speed.velocityMax, frames[0], Align::LEFT);
    creator->add_value("avg  :%4.2f",10,&this->session.speed.avg, frames[1], Align::LEFT);

    TRACE_DEBUG(4, TRACE_VIEWS, "View_Last_Avg render \n");
}

// #------------------------------#
// | static functions definitions |
// #------------------------------#
