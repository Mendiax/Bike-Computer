
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
#include "data_actor.hpp"

// #------------------------------#
// |           macros             |
// #------------------------------#

// #------------------------------#
// | global variables definitions |
// #------------------------------#

View_Session::View_Session(const SessionData& data)
    : data{data}
{
    TRACE_DEBUG(4, TRACE_VIEWS, "View_Session constructor\n");
}

View_Session::~View_Session()
{

}

// start/pause
void View_Session::action(void)
{
    TRACE_DEBUG(2, TRACE_CORE_1, "pause btn pressed \n");

    const auto& sessionDataDisplay = Display_Actor::get_instance().get_local_data().session;
     switch(sessionDataDisplay.get_status())
    {
        case Session::Status::PAUSED:
            {
                Signal sig(actors_common::SIG_DATA_ACTOR_CONTINUE);
                Data_Actor::get_instance().send_signal(sig);
                TRACE_DEBUG(2, TRACE_CORE_1, "sending signal continue %d\n", (int) sig.get_sig_id());

            }
            break;
        case Session::Status::RUNNING:
            {
                Signal sig(actors_common::SIG_DATA_ACTOR_PAUSE);
                Data_Actor::get_instance().send_signal(sig);
                TRACE_DEBUG(2, TRACE_CORE_1, "sending signal pause %d\n", (int) sig.get_sig_id());

            }
            break;
        case Session::Status::NOT_STARTED:
            {
                Signal sig(actors_common::SIG_DATA_ACTOR_SESSION_START);
                Data_Actor::get_instance().send_signal(sig);
                TRACE_DEBUG(2, TRACE_CORE_1, "sending signal start %d\n", (int) sig.get_sig_id());
            }
            break;
        default:
            TRACE_ABNORMAL(TRACE_CORE_1, "pause has no effect %d\n", (int) sessionDataDisplay.get_status());
            break;
    }
}

// end session
void View_Session::action_long(void)
{
    PRINT("end session save");
    auto payload = new Display_Actor::Sig_Display_Actor_End_Session();
    payload->save = true;
    Signal sig(actors_common::SIG_DATA_ACTOR_STOP, payload);
    Data_Actor::get_instance().send_signal(sig);
    Gui::get_gui()->go_back();
}

void  View_Session::action_second_long(void)
{
    PRINT("end session no save");
    auto payload = new Display_Actor::Sig_Display_Actor_End_Session();
    payload->save = false;
    Signal sig(actors_common::SIG_DATA_ACTOR_STOP, payload);
    Data_Actor::get_instance().send_signal(sig);
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
