
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


    Frame legend = {0, frame.y, top.x, frame.height};
    auto legend_frames = View_Creator::split_horizontal_arr(legend, 5);

    const display::DisplayColor color_wind{0,0xf,0};
    const display::DisplayColor color_wind_max{0xf,0x0,0x1};
    const display::DisplayColor color_winddirection{0xf,0,0xf};
    const display::DisplayColor color_rain{0,0,0xf};
    const display::DisplayColor color_temperature{0xf,0xa,0};

    // creator->add_label("h", legend, Align::CENTER);


    creator->add_label("wind  ", legend_frames.at(0), Align::LEFT, 6, color_wind);
    creator->add_label("wind_m", legend_frames.at(1), Align::LEFT, 6, color_wind_max);
    creator->add_label("wind_d", legend_frames.at(2), Align::LEFT, 6, color_winddirection);
    creator->add_label("rain  ", legend_frames.at(3), Align::LEFT, 6, color_rain);
    creator->add_label("temp  ", legend_frames.at(4), Align::LEFT, 6, color_temperature);




    static float min = 0.0, max = 40.0;
    {
        PlotSettings plot_sett{top, false, false, &min, &max,
                               &this->data.forecast.windgusts_10m.array,
                               FORECAST_SENSOR_DATA_LEN, color_wind_max};
        Settings set;
        set.plot = plot_sett;
        Window plot_win{set, plot_float};
        creator->add_new_window(plot_win);
    }
    {
        PlotSettings plot_sett{top, false,false,&min, &max,
        &this->data.forecast.windspeed_10m.array,
        FORECAST_SENSOR_DATA_LEN, color_wind};
        Settings set;
        set.plot = plot_sett;
        Window plot_win{set, plot_float};
        creator->add_new_window(plot_win);
    }
    {
        static float min = 0.0, max = 360.0;
        PlotSettings plot_sett{top, false, false, &min, &max,
                               &this->data.forecast.winddirection_10m.array,
                               FORECAST_SENSOR_DATA_LEN, color_rain};
        Settings set;
        set.plot = plot_sett;
        Window plot_win{set, plot_float};
        creator->add_new_window(plot_win);
    }
    {
        static float min = 0.0, max = 30.0;
        PlotSettings plot_sett{top, false, false, &min, &max,
                               &this->data.forecast.temperature_2m.array,
                               FORECAST_SENSOR_DATA_LEN, color_rain};
        Settings set;
        set.plot = plot_sett;
        Window plot_win{set, plot_float};
        creator->add_new_window(plot_win);
    }
    {
        static float min = 0.0, max = 3.0;
        PlotSettings plot_sett{top, false, false, &min, &max,
                               &this->data.forecast.precipitation.array,
                               FORECAST_SENSOR_DATA_LEN, color_rain};
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
