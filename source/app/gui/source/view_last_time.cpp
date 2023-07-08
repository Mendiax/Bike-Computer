
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
void View_Last_Time::render(void)
{
    auto creator = View_Creator::get_view();
    creator->reset();
    auto frame = creator->setup_bar(&this->data.current_time.hours, &this->data.lipo);


    auto [frame_time, frame_distance] = View_Creator::split_horizontal(frame);

    creator->add_value("time:%2d:%02d:%02d", 13, (mtime_t *)&this->session.speed.drive_time, frame_time, Align::CENTER);
    creator->addValueValuesVertical("dist:%3" PRIu16, 8, &this->session.speed.distance,
                           "%02" PRIu8, 2, &this->session.speed.distanceDec,
                           "km", 2, (void *)0,
                           frame_distance,
                           Align::CENTER, true);

    TRACE_DEBUG(4, TRACE_VIEWS, "View_Last_Time render \n");
}

// #------------------------------#
// | static functions definitions |
// #------------------------------#
