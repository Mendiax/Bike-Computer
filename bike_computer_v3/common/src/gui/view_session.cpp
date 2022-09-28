
// #------------------------------#
// |          includes            |
// #------------------------------#
// pico includes

// c/c++ includes

// my includes
#include "gui/view_session.hpp"
#include "traces.h"
#include "common_actors.hpp"

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

// start/pause
void View_Session::action(void)
{
    Signal sig(SIG_CORE1_START_PAUSE_BTN);
    actor_core1.send_signal(sig);
}

// end session
void View_Session::action_long(void)
{
    Signal sig(SIG_CORE1_END_BTN);
    actor_core1.send_signal(sig);
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
