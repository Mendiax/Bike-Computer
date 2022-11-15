
// #------------------------------#
// |          includes            |
// #------------------------------#
// pico includes

// c/c++ includes
#include <tuple>

// my includes
#include "gui/view_date.hpp"
#include "views/view.hpp"

// #------------------------------#
// |           macros             |
// #------------------------------#

// #------------------------------#
// | global variables definitions |
// #------------------------------#
void View_Date::render(void)
{
    auto creator = View_Creator::get_view();
    creator->reset();
    auto frame = creator->setup_bar(&this->data.current_time.hours, &this->data.lipo);

    auto [frame_hour_date, frame_sunrise_sunset] = View_Creator::split_horizontal(frame);


    auto [frame_hour, frame_date] = View_Creator::split_horizontal(frame_hour_date);
    creator->add_value("", TIMES_LABEL_LENGTH, &data.current_time.hours, frame_hour, Align::CENTER);
    creator->add_value("", TIMES_LABEL_LENGTH, &data.current_time.date, frame_date, Align::CENTER);

    auto [frame_sunrise, frame_sunset] = View_Creator::split_vertical(frame_sunrise_sunset);
    creator->add_value("", TIMES_LABEL_LENGTH - 3, &data.forecast.sunrise, frame_sunrise, Align::CENTER);
    creator->add_value("", TIMES_LABEL_LENGTH - 3, &data.forecast.sunset, frame_sunset, Align::CENTER);
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
