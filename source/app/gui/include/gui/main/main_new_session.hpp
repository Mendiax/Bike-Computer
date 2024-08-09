#ifndef __MAIN_NEW_SESSION_HPP__
#define __MAIN_NEW_SESSION_HPP__
// #-------------------------------#
// |           includes            |
// #-------------------------------#
// pico includes

// c/c++ includes

// my includes
#include "gui_common.hpp"


// #-------------------------------#
// |            macros             |
// #-------------------------------#

// #-------------------------------#
// | global types declarations     |
// #-------------------------------#
class Main_New_Session : public gui::View
{
private:


public:
    using View::View;
    // Main_New_Session() = default;
    /**
     * @brief render function that renders current view
     *
     */
    void render(void);

    /**
     * @brief start new session
     *
     */
    void action(void);
};

// #-------------------------------#
// | global variables declarations |
// #-------------------------------#

// #-------------------------------#
// | global function declarations  |
// #-------------------------------#

#endif
