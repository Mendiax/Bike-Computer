#ifndef __COMMON_GUI_STRUCTURE_HPP__
#define __COMMON_GUI_STRUCTURE_HPP__
// #-------------------------------#
// |           includes            |
// #-------------------------------#
// pico includes

// c/c++ includes

// my includes
#include "gui_common.hpp"
#include "common_types.h"
#include "session.hpp"
#include "massert.hpp"


// #-------------------------------#
// |            macros             |
// #-------------------------------#

// #-------------------------------#
// | global types declarations     |
// #-------------------------------#
/**
 * @brief singleton class for gui handling
 *
 */
class Gui
{
private:
    static Gui* singleton;

    gui::View_List* current_view_list;

    Sensor_Data* data;
    Session_Data* session;

    /**
     * @brief sets all views and structure of gui should be called only once
     *
     */
    void create();

    void set_data(Sensor_Data* data_p, Session_Data* session_p);

    Gui(Sensor_Data* data_p, Session_Data* session_p);
    ~Gui();
public:
    static inline Gui* get_gui(Sensor_Data* data_p = nullptr, Session_Data* session_p = nullptr)
    {
        if(singleton == nullptr)
        {
            massert(data_p != nullptr, "get_gui create with nullptr parameters\n");
            massert(session_p != nullptr, "get_gui create with nullptr parameters\n");

            singleton = new Gui(data_p, session_p);
        }
        return singleton;
    }
    /**
     * @brief executs buttons handlers
     *
     */
    void handle_buttons();
    /**
     * @brief chenge view to the next in the list
     *
     */
    void go_next();
    void go_prev();

    /**
     * @brief move one level up if exists
     *
     */
    void go_back();
    /**
     * @brief enters one level in if exists
     *
     */
    void enter();

    /**
     * @brief Get the current view
     *
     * @return View*
     */
    gui::View* get_current();

    /**
     * @brief refreshes data of view (only calls window update for each window)
     *
     */
    void refresh();
    /**
     * @brief call setup function for view
     *
     */
    void render();
};
// #-------------------------------#
// | global variables declarations |
// #-------------------------------#

// #-------------------------------#
// | global function declarations  |
// #-------------------------------#

#endif
