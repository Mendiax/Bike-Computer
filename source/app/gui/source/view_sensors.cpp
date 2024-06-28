
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

    auto frames = View_Creator::split_horizontal_arr(frame, 5);
    creator->add_value("voltage:%.2fV",std::strlen("voltage:") + 4,&this->data.lipo.voltage, frames[0], Align::LEFT);
    #define FORMAT "% .2f"
    #define FORMAT_I "% 4" PRIi16
    #define LEN 6


    auto frame_imu_rot_arr = View_Creator::split_vertical_arr(frames[1], 3);
    creator->add_value("X" FORMAT, LEN,&this->data.imu.rotation.x,frame_imu_rot_arr[0], Align::CENTER);
    creator->add_value("Y" FORMAT, LEN,&this->data.imu.rotation.y,frame_imu_rot_arr[1], Align::CENTER);
    creator->add_value("Z" FORMAT, LEN,&this->data.imu.rotation.z,frame_imu_rot_arr[2], Align::CENTER);

    auto frame_imu_rot_speed_arr = View_Creator::split_vertical_arr(frames[2], 3);
    creator->add_value("X" FORMAT, LEN,&this->data.imu.rotation_speed.x,frame_imu_rot_speed_arr[0], Align::CENTER);
    creator->add_value("Y" FORMAT, LEN,&this->data.imu.rotation_speed.y,frame_imu_rot_speed_arr[1], Align::CENTER);
    creator->add_value("Z" FORMAT, LEN,&this->data.imu.rotation_speed.z,frame_imu_rot_speed_arr[2], Align::CENTER);

    auto frame_imu_accel_arr = View_Creator::split_vertical_arr(frames[3], 3);
    creator->add_value("X" FORMAT_I, LEN,&this->data.imu.accelerometer.x,frame_imu_accel_arr[0], Align::CENTER);
    creator->add_value("Y" FORMAT_I, LEN,&this->data.imu.accelerometer.y,frame_imu_accel_arr[1], Align::CENTER);
    creator->add_value("Z" FORMAT_I, LEN,&this->data.imu.accelerometer.z,frame_imu_accel_arr[2], Align::CENTER);

    auto frame_imu_mag_arr = View_Creator::split_vertical_arr(frames[4], 3);
    creator->add_value("X" FORMAT_I, LEN,&this->data.imu.magnetometer.x,frame_imu_mag_arr[0], Align::CENTER);
    creator->add_value("Y" FORMAT_I, LEN,&this->data.imu.magnetometer.y,frame_imu_mag_arr[1], Align::CENTER);
    creator->add_value("Z" FORMAT_I, LEN,&this->data.imu.magnetometer.z,frame_imu_mag_arr[2], Align::CENTER);

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
