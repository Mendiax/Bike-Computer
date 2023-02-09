
// #------------------------------#
// |          includes            |
// #------------------------------#
// pico includes

// c/c++ includes
#include <tuple>

// my includes
#include "views/view.hpp"
#include "gui/view_total.hpp"

// #------------------------------#
// |           macros             |
// #------------------------------#

// #------------------------------#
// | global variables definitions |
// #------------------------------#
void View_Total::render(void)
{
    auto creator = View_Creator::get_view();
    creator->reset();
    auto frame = creator->setup_bar(&this->data.current_time.hours, &this->data.lipo);

    auto [frame_time, frame_dist] = View_Creator::split_horizontal(frame);
    creator->add_value("time:%.1f",(5+5),&this->data.total_time_ridden, frame_time, Align::LEFT);
    creator->add_value("dist:%.1f",(5+5),&this->data.total_distance_ridden, frame_dist, Align::LEFT);
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
