
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
    auto frame = creator->setup_bar(&this->data);
    GpsSettings settings;
    settings.frame = frame;
    settings.color = COLOR_WHITE;
    settings.color2 = COLOR_BLUE;
    settings.data = &this->data.gps_graph;
    settings.radius = 1.0f; // ~1km
    creator->add_gps(settings);


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
