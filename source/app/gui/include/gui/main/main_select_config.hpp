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
#include "persistent_storage.hpp"

// #-------------------------------#
// |            macros            |
// #-------------------------------#

#define NO_DISPLAYED_CONFIG 3
#define MAX_CONFIG_NAME_LEN 15

// #-------------------------------#
// | global types declarations    |
// #-------------------------------#
class Main_Select_Config : public gui::View
{
private:
    std::array<std::array<char, MAX_CONFIG_NAME_LEN + 1>, NO_DISPLAYED_CONFIG> displayed_data;
    size_t current_config_idx;
    size_t no_configs;
    std::vector<std::string> all_config_files;

    void insert_files_into_display(void);

public:
    Main_Select_Config(gui::View_List* p);

    /**
     * @brief render function that renders current view
     */
    void render(void);

    /**
     * @brief next file
     */
    void action(void);

    /**
     * @brief accept file and load config
     */
    void action_long(void);

    /**
     * @brief previous file
     */
    void action_second(void);
};
