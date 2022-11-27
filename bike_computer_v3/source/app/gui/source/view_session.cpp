
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
#include "display_actor.hpp"

// #------------------------------#
// |           macros             |
// #------------------------------#

// #------------------------------#
// | global variables definitions |
// #------------------------------#

View_Session::View_Session(const Sensor_Data& data, const Session_Data& session)
    : data{data}, session{session}
{
    TRACE_DEBUG(4, TRACE_VIEWS, "View_Session constructor\n");
}

View_Session::~View_Session()
{

}

// start/pause
void View_Session::action(void)
{
    Signal sig(actors_common::SIG_DISPLAY_ACTOR_START_PAUSE_BTN);
    Display_Actor::get_instance().send_signal(sig);
}

// end session
void View_Session::action_long(void)
{
    auto payload = new Display_Actor::Sig_Display_Actor_End_Sesion();
    payload->save = true;
    Signal sig(actors_common::SIG_DISPLAY_ACTOR_END_BTN, payload);
    Display_Actor::get_instance().send_signal(sig);
    Gui::get_gui()->go_back();
}

void  View_Session::action_second_long(void)
{
    auto payload = new Display_Actor::Sig_Display_Actor_End_Sesion();
    payload->save = false;
    Signal sig(actors_common::SIG_DISPLAY_ACTOR_END_BTN, payload);
    Display_Actor::get_instance().send_signal(sig);
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
