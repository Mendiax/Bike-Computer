
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

    // const auto frame = get_frame_bar();

    // const auto [frame_speed, frame_distance] = split_horizontal();

    Frame speed = {0, TOP_BAR_HEIGHT, DISPLAY_WIDTH, DISPLAY_HEIGHT/2};

    // addValueUnitsVertical("%2.0f", 2, &this->data.speed.velocity, "km","h", speed,
    //                        Align::CENTER, true);
    creator->addValueValuesVertical("%2.0f", (size_t)2, &this->data.velocity, // here
                                    "%3.0f", (size_t)3, &this->data.cadence,
                                    "%3" PRIu8, (size_t)3, &this->data.gear.rear,
                                    //"kph", 3, (void*)0,
                                    speed,
                                    Align::RIGHT, false);

    Frame distanceFrame = {0, View_Creator::get_frame_top_y(speed), DISPLAY_WIDTH, DISPLAY_HEIGHT/2}; // here
    creator->addValueValuesVertical("%3" PRIu16, 3, &this->session.speed.distance, // here
                           "%02" PRIu8, 2, &this->session.speed.distanceDec,
                           "km", 2, (void *)0,
                           distanceFrame,
                           Align::CENTER, true);

    TRACE_DEBUG(4, TRACE_VIEWS, "view_velocity render \n");
}

// #------------------------------#
// | static functions definitions |
// #------------------------------#
