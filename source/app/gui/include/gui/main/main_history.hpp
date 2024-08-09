#ifndef __MAIN_MAIN_HISTORY_HPP__
#define __MAIN_MAIN_HISTORY_HPP__
// #-------------------------------#
// |           includes            |
// #-------------------------------#
// pico includes

// c/c++ includes
#include <array>
#include <vector>
#include <string>

// my includes
#include "gui_common.hpp"
#include "sd_file.h"

// #-------------------------------#
// |            macros             |
// #-------------------------------#

#define NO_DISPLAYED_HISTORY 3
#define MAX_LOG_NAME_LEN 15
// #-------------------------------#
// | global types declarations     |
// #-------------------------------#
class Main_History : public gui::View
{
private:

    std::array<char[MAX_LOG_NAME_LEN + 1], NO_DISPLAYED_HISTORY> displayed_data;
    // std::vector<std::string> all_log_files;
    size_t current_log_idx;
    // Sd_File* sessions;
    size_t no_sessions;

    void insert_files_into_display(void);
    // std::string extract_session_name(size_t id);

public:
    // using View::View;
    Main_History(gui::View_List* p);
    // Main_New_Session() = default;
    /**
     * @brief render function that renders current view
     *
     */
    void render(void);

    /**
     * @brief next file
     *
     */
    void action(void);

    /**
     * @brief accept file
     *
     */
    void action_long(void);

    void action_second(void);
};
// #-------------------------------#
// | global variables declarations |
// #-------------------------------#

// #-------------------------------#
// | global function declarations  |
// #-------------------------------#

#endif
