
// #------------------------------#
// |          includes            |
// #------------------------------#
// pico includes

// c/c++ includes
#include <tuple>

// my includes
#include "gui/view_gps.hpp"
#include "views/view.hpp"

// #------------------------------#
// |           macros             |
// #------------------------------#

// #------------------------------#
// | global variables definitions |
// #------------------------------#
void View_Gps::render(void)
{
    auto creator = View_Creator::get_view();
    creator->reset();
    auto frame = creator->setup_bar(&this->data.current_time.hours, &this->data.lipo);
    auto [gps, gsm] = View_Creator::split_horizontal(frame);


    auto [gps_speed_height, gps_pos] = View_Creator::split_horizontal(gps);
    auto [gps_speed, gps_height] = View_Creator::split_vertical(gps_speed_height);
    auto [gps_lat, gps_long] = View_Creator::split_vertical(gps_pos);
    auto [signal, bmp] = View_Creator::split_vertical(gsm);
    auto [sat, sat2] = View_Creator::split_horizontal(signal);
    auto [temp, slope] = View_Creator::split_horizontal(bmp);



    //Frame topLeft = {0,TOP_BAR_HEIGHT, DISPLAY_WIDTH/2, DISPLAY_HEIGHT/2};
    //Frame topRight = {DISPLAY_WIDTH/2,topLeft.y, DISPLAY_WIDTH/2, topLeft.height};
    creator->addValueUnitsVertical("%2.0f", 2, &this->data.gps_data.speed, "km","h", gps_speed,
                           Align::CENTER, true);
    //add_value("%3.0f",3,&this->data.gps_data.msl, gps_height, Align::CENTER);
    creator->add_value("%3.0fm",4,&this->data.altitude, gps_height, Align::CENTER);

    //Frame bottomLeft ={0, get_frame_top_y(topLeft), topLeft.width, DISPLAY_HEIGHT - bottomLeft.y};
    //Frame bottomRight ={topRight.x,get_frame_top_y(topRight), topRight.width, bottomRight.height};
    creator->add_value("%6.3fN",7,&this->data.gps_data.lat, gps_lat, Align::CENTER);
    creator->add_value("%6.3fE",7,&this->data.gps_data.lon, gps_long, Align::CENTER);


    creator->add_value("s1:%2" PRIu8,5,&this->data.gps_data.sat, sat, Align::LEFT);
    creator->add_value("s2:%2" PRIu8,5,&this->data.gps_data.sat2, sat2, Align::LEFT);

    creator->add_value("% 3.0fC",4,&this->data.weather.temperature, temp, Align::CENTER);
    creator->add_value("% 3.0fp",4,&this->data.slope, slope, Align::CENTER); // TODO fix for %
    //add_value("%4f",4,&this->data.gps_data.sat2, sat2, Align::CENTER);
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
