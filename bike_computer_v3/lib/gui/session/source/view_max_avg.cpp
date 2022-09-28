
// #------------------------------#
// |          includes            |
// #------------------------------#
// pico includes

// c/c++ includes
#include <tuple>

// my includes
#include "view_max_avg.hpp"
#include "views/view.hpp"

// #------------------------------#
// |           macros             |
// #------------------------------#

// #------------------------------#
// | global variables definitions |
// #------------------------------#
void View_Max_Avg::render(void)
{
    auto creator = View_Creator::get_view();
    creator->reset();
    auto frame = creator->setup_bar(&this->data.current_time.hours, &this->data.lipo);

    Frame max_avg_bar = {0, TOP_BAR_HEIGHT, DISPLAY_WIDTH, DISPLAY_HEIGHT/5};
    Frame max_avg_val = {0, View_Creator::get_frame_top_y(max_avg_bar), DISPLAY_WIDTH, 100};
    Frame time_passed = {0, View_Creator::get_frame_top_y(max_avg_val), DISPLAY_WIDTH, View_Creator::get_height_left(max_avg_val)};

    auto [bar_max,bar_avg] = View_Creator::split_vertical(max_avg_bar);
    creator->add_label("max", bar_max, Align::CENTER);
    creator->add_label("avg", bar_avg, Align::CENTER);


    auto [max,avg] = View_Creator::split_vertical(max_avg_val);
    creator->add_value("%2.0f", 2, &this->session.speed.velocityMax, max, Align::CENTER);

    auto [avg_curr, avg_global] = View_Creator::split_horizontal(avg);
    creator->add_value("%4.1f",4,&this->session.speed.avg, avg_curr, Align::LEFT);
    creator->add_value("%4.1f",4,&this->session.speed.avg_global, avg_global, Align::LEFT);


    // Frame time_passed = {0, bottomLeft.y + bottomLeft.height, DISPLAY_WIDTH, DISPLAY_HEIGHT - time_passed.y};
    creator->add_value("%2d:%02d:%02d", 8, (mtime_t *)&this->session.speed.drive_time, time_passed, Align::CENTER);

    TRACE_DEBUG(4, TRACE_VIEWS, "view1 setup finished \n%s", "");
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
