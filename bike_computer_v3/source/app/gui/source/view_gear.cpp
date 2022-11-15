
// #------------------------------#
// |          includes            |
// #------------------------------#
// pico includes

// c/c++ includes
#include <tuple>

// my includes
#include "views/view.hpp"
#include "gui/view_gear.hpp"

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
void View_Gear::render(void)
{
    auto creator = View_Creator::get_view();
    creator->reset();
    auto frame = creator->setup_bar(&this->data.current_time.hours, &this->data.lipo);

    /*

    speed;  accel
    gear; sugg
    cadence opt range
    */

    frame.height = DISPLAY_HEIGHT;

    auto frames = View_Creator::split_horizontal_arr(frame, 3);

    const auto& frame_speed = frames.at(0);
    auto [frame_velocity, frame_accel] = View_Creator::split_vertical(frame_speed);
    creator->add_value("%2.0f", (size_t)2, &this->data.velocity, frame_velocity, Align::CENTER);
    creator->add_value("% 3.1f", (size_t)4, &this->data.accel, frame_accel, Align::RIGHT);


    const auto& frame_gears = frames.at(1);
    auto [frame_gear, frame_gear_suggestion] = View_Creator::split_vertical(frame_gears);
    creator->add_value("%2" PRIu8, (size_t)2, &this->data.gear.rear, frame_gear, Align::CENTER);
    creator->add_label(this->data.gear_suggestions.gear_suggestion, frame_gear_suggestion, Align::RIGHT, (size_t)2);
    auto prev = creator->get_previous_window();
    prev->settings.label.text.color = &this->data.gear_suggestions.gear_suggestion_color;


    const auto& frame_cadence_stats = frames.at(2);
    auto [frame_cadence, frame_cadence_range] = View_Creator::split_vertical(frame_cadence_stats);
    auto [frame_cadence_min, frame_cadence_max] = View_Creator::split_vertical(frame_cadence_range);
    creator->add_value("%3.0f", (size_t)3, &this->data.cadence, frame_cadence, Align::RIGHT);

    creator->add_value("%3.0f", (size_t)3, &this->data.gear_suggestions.cadence_min, frame_cadence_min, Align::RIGHT);
    creator->add_value("%3.0f", (size_t)3, &this->data.gear_suggestions.cadence_max, frame_cadence_max, Align::RIGHT);

}

// #------------------------------#
// | static functions definitions |
// #------------------------------#
