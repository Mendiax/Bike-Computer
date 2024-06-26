#ifndef __session_VIEW_SESSION_HISTORY_HPP__
#define __session_VIEW_SESSION_HISTORY_HPP__
// #-------------------------------#
// |           includes            |
// #-------------------------------#
// pico includes

// c/c++ includes

// my includes
#include "gui_common.hpp"
#include "views/view.hpp"
#include "common_types.h"
#include "session.hpp"


// #-------------------------------#
// |            macros             |
// #-------------------------------#

// #-------------------------------#
// | global types declarations     |
// #-------------------------------#
/**
 * @brief class that display data of current session
 *
 */
class View_Session_History : public gui::View
{
protected:
    const Sensor_Data& data;
    const Session_Data& session;

public:
    View_Session_History(const Sensor_Data& data, const Session_Data& session);
    ~View_Session_History();

    // start/pause
    void action(void);
    // end session
    void action_long(void);
};

// #-------------------------------#
// | global variables declarations |
// #-------------------------------#

// #-------------------------------#
// | global function declarations  |
// #-------------------------------#

#endif
