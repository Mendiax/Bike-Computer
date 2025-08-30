#ifndef __session_VIEW_SESSION_HPP__
#define __session_VIEW_SESSION_HPP__
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
class View_Session : public gui::View
{
protected:
    const SessionData& data;
public:
    View_Session(const SessionData& data);
    ~View_Session();

    // start/pause
    virtual void action(void);
    // end session with save
    virtual void action_long(void);
    // Base action_second implementation
    // virtual void action_second(void);
    // end session no save
    virtual void action_second_long(void);
};

// #-------------------------------#
// | global variables declarations |
// #-------------------------------#

// #-------------------------------#
// | global function declarations  |
// #-------------------------------#

#endif
