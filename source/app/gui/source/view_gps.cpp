
// #------------------------------#
// |          includes            |
// #------------------------------#
// pico includes

// c/c++ includes
#include <tuple>

// my includes
#include "gui/view_gps.hpp"
#include "views/view.hpp"
#include "display/driver.hpp"
#include "global.hpp"


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
    auto frame = creator->setup_bar(&this->data.sensors);

    const float radius = GET_GLOBAL(global_gps_radius, 0.001f); // ~1km

    {
        GpsSettings settings;
        settings.frame = frame;
        settings.color = COLOR_WHITE;
        settings.color2 = COLOR_BLUE;
        settings.data = &this->data.sensors.gps_graph.points;
        settings.pos = &this->data.sensors.gps_graph.pos;
        settings.radius = radius;
        creator->add_gps(settings);
    }

    {
        GpsSettings settings;
        settings.frame = frame;
        settings.color = COLOR_RED;
        settings.color2 = COLOR_GREEN;
        settings.data = &this->data.track.track_graph.points;
        settings.pos = &this->data.sensors.gps_graph.pos;
        settings.radius = radius;
        creator->add_gps(settings);
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
