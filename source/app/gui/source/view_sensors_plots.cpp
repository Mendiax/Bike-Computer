
// #------------------------------#
// |          includes            |
// #------------------------------#
// pico includes

// c/c++ includes
#include <cstring>
#include <tuple>

// my includes
#include "display/driver.hpp"
#include "views/view.hpp"
#include "gui/view_sensors_plots.hpp"

// #------------------------------#
// |           macros             |
// #------------------------------#

// #------------------------------#
// | global variables definitions |
// #------------------------------#
void View_Sensors_Plots::render(void)
{
    auto creator = View_Creator::get_view();
    creator->reset();
    auto frame = creator->setup_bar(&this->data.current_time.hours, &this->data.lipo);
    // static float val_min=0;
    // static float val_max=0;

    static PlotSettings settings = {
        .frame = frame,
        .auto_max=true,
        .auto_min=true,
        .min = nullptr,
        .max = nullptr,
        .data = &this->data.imu.accel_hist,
        .len = 100,
        .color = COLOR_WHITE
    };
    creator->add_plot_ring<float>(settings);

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
