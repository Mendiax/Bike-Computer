
// #------------------------------#
// |          includes            |
// #------------------------------#
// pico includes

// c/c++ includes

// my includes
#include "view_session.hpp"
#include "traces.h"

// #------------------------------#
// |           macros             |
// #------------------------------#

// #------------------------------#
// | global variables definitions |
// #------------------------------#

View_Session::View_Session(const Sensor_Data& data, const Session_Data& session, bool view_only)
    : view_only{view_only}, data{data}, session{session}
{
    TRACE_DEBUG(4, TRACE_VIEWS, "View_Session constructor\n");
}

View_Session::~View_Session()
{

}



// void View_Session::action(void)
// {

// }

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
