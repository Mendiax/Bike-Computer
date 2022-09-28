#ifndef __session_VIEW_VELOCITY_HPP__
#define __session_VIEW_VELOCITY_HPP__
// #-------------------------------#
// |           includes            |
// #-------------------------------#
// pico includes

// c/c++ includes

// my includes
#include "view_session.hpp"

// #-------------------------------#
// |            macros             |
// #-------------------------------#

// #-------------------------------#
// | global types declarations     |
// #-------------------------------#

class View_Velocity : public View_Session
{
private:


public:
    using View_Session::View_Session;
    // ~View_Velocity();

    /**
     * @brief render function that renders current view
     *
     */
    void render(void);
    // /**
    //  * @brief special function that executes when special button is pressed
    //  * Mainly used to go to the next view
    //  *
    //  */
    // void action(void);
};

// #-------------------------------#
// | global variables declarations |
// #-------------------------------#

// #-------------------------------#
// | global function declarations  |
// #-------------------------------#

#endif
