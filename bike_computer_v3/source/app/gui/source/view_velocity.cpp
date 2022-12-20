
// #------------------------------#
// |          includes            |
// #------------------------------#
// pico includes

// c/c++ includes
#include <tuple>

// my includes
#include "views/view.hpp"
#include "gui/view_velocity.hpp"

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
void View_Velocity::render(void)
{
    auto creator = View_Creator::get_view();
    creator->reset();
    auto frame = creator->setup_bar(&this->data.current_time.hours, &this->data.lipo);

    frame.height = DISPLAY_HEIGHT;

    auto [frame_speed, frame_distance] = View_Creator::split_horizontal(frame);
    auto [frame_velocity, frame_stats] = View_Creator::split_vertical(frame_speed, 2);
    frame_velocity.width += 10;

    auto [frame_cadence, frame_gear] = View_Creator::split_horizontal(frame_stats);
    auto [frame_gear_suggestion, frame_gear_current] = View_Creator::split_vertical(frame_gear);
    frame_gear_current.width += 5;
    frame_gear_current.x -= 5;

    creator->add_value("%2.0f", (size_t)2, &this->data.velocity, frame_velocity, Align::LEFT);
    creator->add_value("%3.0f", (size_t)3, &this->data.cadence, frame_cadence, Align::RIGHT);
    creator->add_value("%3" PRIu8, (size_t)3, &this->data.gear.rear, frame_gear, Align::RIGHT);
    creator->add_label(this->data.gear_suggestions.gear_suggestion, frame_gear_suggestion, Align::RIGHT, (size_t)2);

    auto prev = creator->get_previous_window();
    prev->settings.label.text.color = &this->data.gear_suggestions.gear_suggestion_color;
    creator->addValueValuesVertical("%3" PRIu16, 3, &this->session.speed.distance,
                           "%02" PRIu8, 2, &this->session.speed.distanceDec,
                           "km", 2, (void *)0,
                           frame_distance,
                           Align::CENTER, true);

    TRACE_DEBUG(4, TRACE_VIEWS, "view_velocity render \n");
}

// #------------------------------#
// | static functions definitions |
// #------------------------------#
