
// #------------------------------#
// |          includes            |
// #------------------------------#
// pico includes

// c/c++ includes
#include <tuple>

// my includes
#include "common_types.h"
#include "gui/view_last_date.hpp"
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
void View_Last_Date::render(void)
{
    auto creator = View_Creator::get_view();
    creator->reset();
    auto frame = creator->setup_bar(&this->data.current_time.hours, &this->data.lipo);

    // frame.height = DISPLAY_HEIGHT;

    auto [frame_start, frame_end] = View_Creator::split_horizontal(frame);

    {
        auto [frame_hour, frame_date] = View_Creator::split_horizontal(frame_start);
        creator->add_value("start: ", TIMES_LABEL_LENGTH + 7, &this->session.time_start.date, frame_hour, Align::LEFT);
        creator->add_value("       ", TIMES_LABEL_LENGTH + 7, &this->session.time_start.hours, frame_date, Align::LEFT);
    }
    {
        auto [frame_hour, frame_date] = View_Creator::split_horizontal(frame_end);
        creator->add_value("end  : ", TIMES_LABEL_LENGTH + 7, &this->session.time_end.date, frame_hour, Align::LEFT);
        creator->add_value("       ", TIMES_LABEL_LENGTH + 7, &this->session.time_end.hours, frame_date, Align::LEFT);
    }

    TRACE_DEBUG(4, TRACE_VIEWS, "View_Last_Date render \n");
}

// #------------------------------#
// | static functions definitions |
// #------------------------------#
