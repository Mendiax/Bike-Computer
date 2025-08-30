#pragma once
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
#define MAX_TRACK_NAME_LEN 15
// #-------------------------------#
// | global types declarations     |
// #-------------------------------#
class Main_Select_Tracks : public gui::View
{
private:

    std::array<std::array<char, MAX_TRACK_NAME_LEN + 1>, NO_DISPLAYED_HISTORY> displayed_data;
    size_t current_log_idx;
    // Sd_File* sessions;
    size_t no_tracks;
    std::vector<std::string> all_tracks_files;

    void insert_files_into_display(void);

public:
    // using View::View;
    Main_Select_Tracks(gui::View_List* p);
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

