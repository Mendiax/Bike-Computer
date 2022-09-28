#ifndef __SESSION_VIEW_DATE_HPP__
#define __SESSION_VIEW_DATE_HPP__
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
class View_Date : public View_Session
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
