
// #------------------------------#
// |          includes            |
// #------------------------------#
// pico includes

// c/c++ includes
#include <tuple>
// #include <iostream>

// my includes
#include "views/view.hpp"
#include "gui/view_forecast.hpp"
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


    auto [top, bottom] = View_Creator::split_horizontal(frame, 5, true);


    // std::cout << "top: " << frame_to_string(top) << std::endl;
    // std::cout << "bottom: " << frame_to_string(bottom) << std::endl;
    const Frame f = bottom;
    const short* ptr = &this->data.forecast.time_h.array[0];
    const auto w = creator->add_arr_label(ptr,FORECAST_SENSOR_DATA_LEN, f);

    // PRINTF("width: %" PRIu16 "\n", w);
    top.x = top.get_max_x() - w;
    top.width = w;
    creator->get_previous_window()->settings.label.text.offsetX = top.x;

    // TODO
    // Frame legend  = {0, frame.height, top.x, View_Creator::get_height_left(frame)};
    // const auto space_per_label = legend.height / 3;
    // Frame legend_wind = legend;     legend_wind.height = space_per_label;
    // Frame legend_wind_max = legend; legend_wind_max.height = space_per_label; legend_wind_max.y = legend.get_max_y();
    // Frame legend_precipitation = legend; legend_precipitation.height = space_per_label; legend_precipitation.y = legend_wind_max.get_max_y();

    // creator->add_label("wind", legend_wind);
    // creator->add_label("wind_m", legend_wind_max);
    // creator->add_label("percip", legend_precipitation);



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
