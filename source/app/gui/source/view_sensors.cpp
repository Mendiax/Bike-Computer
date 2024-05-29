
// #------------------------------#
// |          includes            |
// #------------------------------#
// pico includes

// c/c++ includes
#include <cstring>
#include <tuple>

// my includes
#include "views/view.hpp"
#include "gui/view_sensors.hpp"

// #------------------------------#
// |           macros             |
// #------------------------------#

// #------------------------------#
// | global variables definitions |
// #------------------------------#
void View_Sensors::render(void)
{
    auto creator = View_Creator::get_view();
    creator->reset();
    auto frame = creator->setup_bar(&this->data.current_time.hours, &this->data.lipo);

    auto [frame_bat, frame_imu] = View_Creator::split_horizontal(frame);
    creator->add_value("voltage:%.2fV",std::strlen("voltage:") + 4,&this->data.lipo.voltage, frame_bat, Align::LEFT);
    auto frame_imu_arr = View_Creator::split_vertical_arr(frame_imu, 3);
    #define FORMAT "%.2f"
    creator->add_value("X " FORMAT, 5,&this->data.imu.rotation.x,frame_imu_arr[0], Align::CENTER);
    creator->add_value("Y " FORMAT, 5,&this->data.imu.rotation.y,frame_imu_arr[1], Align::CENTER);
    creator->add_value("Z " FORMAT, 5,&this->data.imu.rotation.z,frame_imu_arr[2], Align::CENTER);
    #undef FORMAT
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
