
// #------------------------------#
// |          includes            |
// #------------------------------#
// pico includes

// c/c++ includes
#include <tuple>

// my includes
#include "views/view.hpp"
#include "view_forecast.hpp"
#include "views/screenfunc/plot.h"

// #------------------------------#
// |           macros             |
// #------------------------------#

// #------------------------------#
// | global variables definitions |
// #------------------------------#
void View_Forecast::render(void)
{
    auto creator = View_Creator::get_view();
    creator->reset();
    auto frame = creator->setup_bar(&this->data.current_time.hours, &this->data.lipo);

    //std::cout << "frame: " << frame_to_string(frame) << std::endl;
    //std::cout << "height left: " << get_height_left(frame) << std::endl;

    Frame top{0, TOP_BAR_HEIGHT, frame.width, frame.height - (DISPLAY_HEIGHT / 10)};
    Frame top_labels{top.x,top.y,top.width, TOP_BAR_HEIGHT};
    //std::cout << "top: " << frame_to_string(top) << std::endl;
    Frame bottom{0,  View_Creator::get_frame_top_y(top), frame.width,  View_Creator::get_height_left(top)};
    //std::cout << "bottom: " << frame_to_string(bottom) << std::endl;

    static float min = 0.0, max = 40.0;
    {
        PlotSettings plot_sett{top, false,false,&min, &max,
        &this->data.forecast.windgusts_10m.array,
        FORECAST_SENSOR_DATA_LEN, {0xf,0x0,0x1}};
        Settings set;
        set.plot = plot_sett;
        Window plot_win{set, plot_float};
        creator->add_new_window(plot_win);
    }
    {
        PlotSettings plot_sett{top, false,false,&min, &max,
        &this->data.forecast.windspeed_10m.array,
        FORECAST_SENSOR_DATA_LEN, {0,0xf,0}};
        Settings set;
        set.plot = plot_sett;
        Window plot_win{set, plot_float};
        creator->add_new_window(plot_win);
    }
    {
        static float min = 0.0, max = 3.0;
        PlotSettings plot_sett{top, false,false,&min, &max,
        &this->data.forecast.precipitation.array,
        FORECAST_SENSOR_DATA_LEN, {0,0,0xf}};
        Settings set;
        set.plot = plot_sett;
        Window plot_win{set, plot_float};
        creator->add_new_window(plot_win);
    }
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
