#ifndef __SENSORS_VIEW_TOTAL_HPP__
#define __SENSORS_VIEW_TOTAL_HPP__
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
class View_Sensors : public View_Session
{
private:


public:
    using View_Session::View_Session;

    /**
     * @brief render function that renders current view
     *
     */
    void render(void);
};
// #-------------------------------#
// | global variables declarations |
// #-------------------------------#

// #-------------------------------#
// | global function declarations  |
// #-------------------------------#

#endif
