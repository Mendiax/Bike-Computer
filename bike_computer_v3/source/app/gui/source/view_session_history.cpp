
// #------------------------------#
// |          includes            |
// #------------------------------#
// pico includes

// c/c++ includes

// my includes
#include "gui/view_session_history.hpp"
#include "traces.h"
#include "common_actors.hpp"
#include "gui/structure.hpp"
#include "display_actor.hpp"

// #------------------------------#
// |           macros             |
// #------------------------------#

// #------------------------------#
// | global variables definitions |
// #------------------------------#

View_Session_History::View_Session_History(const Sensor_Data& data, const Session_Data& session)
    : data{data}, session{session}
{
    TRACE_DEBUG(4, TRACE_VIEWS, "View_Session_History constructor\n");
}

View_Session_History::~View_Session_History()
{

}

// start/pause
void View_Session_History::action(void)
{
}

// end session
void View_Session_History::action_long(void)
{
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
