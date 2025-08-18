#ifndef __VIEW_BOOT_HPP__
#define __VIEW_BOOT_HPP__
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

class View_Boot : public gui::View
{
protected:
    const Sensor_Data& data;
    const Session_Data& session;

public:
    View_Boot(const Sensor_Data& data, const Session_Data& session, gui::View_List* next);
    /**
     * @brief render function that renders current view
     *
     */
    virtual void render(void);
    virtual void action(void);

};

// #-------------------------------#
// | global variables declarations |
// #-------------------------------#

// #-------------------------------#
// | global function declarations  |
// #-------------------------------#

#endif