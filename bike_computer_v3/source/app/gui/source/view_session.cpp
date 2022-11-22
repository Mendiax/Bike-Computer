
// #------------------------------#
// |          includes            |
// #------------------------------#
// pico includes

// c/c++ includes

// my includes
#include "gui/view_session.hpp"
#include "traces.h"
#include "common_actors.hpp"
#include "gui/structure.hpp"

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
    if(!view_only)
    {
        Signal sig(SIG_DISPLAY_ACTOR_START_PAUSE_BTN);
        display_actor.send_signal(sig);
    }
}

// end session
void View_Session::action_long(void)
{
    if(!view_only)
    {
        Signal sig(SIG_DISPLAY_ACTOR_END_BTN);
        display_actor.send_signal(sig);
    }
    Gui::get_gui()->go_back();
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
