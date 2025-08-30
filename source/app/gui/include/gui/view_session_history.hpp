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
#include "session_data.hpp"


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
    const SessionData& data;

public:
    View_Session_History(const SessionData& data);
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
