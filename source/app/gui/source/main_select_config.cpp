// #------------------------------#
// |          includes            |
// #------------------------------#
// pico includes

// c/c++ includes
#include <string.h>

// my includes
#include "gui/main/main_select_config.hpp"
#include "gui/structure.hpp"
#include "views/view.hpp"
#include "traces.h"
#include "sd_common.h"
#include "utils.hpp"
#include "common_actors.hpp"
#include "display_actor.hpp"
#include "data_actor.hpp"

// #------------------------------#
// |           macros             |
// #------------------------------#

#define FOLDER_CONFIG_DATA SD_PATH "config/"

// #------------------------------#
// | global variables definitions |
// #------------------------------#

// #------------------------------#
// | static variables definitions |
// #------------------------------#

// #------------------------------#
// | static functions declarations|
// #------------------------------#

// #------------------------------#
// | global function definitions  |
// #------------------------------#
Main_Select_Config::Main_Select_Config(gui::View_List* p)
    :gui::View(p)
{
}

void Main_Select_Config::insert_files_into_display(void)
{
    TRACE_DEBUG(2, TRACE_GUI, "no_configs = %zu\n", no_configs);

    size_t data_idx = this->current_config_idx;
    for(auto& f : this->displayed_data)
    {
        if(data_idx <= no_configs)
        {
            auto config_name = all_config_files.at(data_idx - 1);
            strncpy(f.data(), config_name.c_str(), MAX_CONFIG_NAME_LEN);
            f[MAX_CONFIG_NAME_LEN] = '\0';
            TRACE_DEBUG(2, TRACE_GUI, "id:%zu, config:%s\n", data_idx, f.data());
            data_idx++;
        }
        else
        {
            strcpy(f.data(), "");
        }
    }

    for(size_t idx = 0; idx < NO_DISPLAYED_CONFIG; idx++)
    {
        TRACE_DEBUG(2, TRACE_GUI, "Data[%zu] %s\n", idx, displayed_data.at(idx).data());
    }
}

void Main_Select_Config::render()
{
    this->current_config_idx = 1;

    // insert data into displayed_data
    all_config_files = dir::get_files(FOLDER_CONFIG_DATA);
    TRACE_DEBUG(2, TRACE_GUI, "all_config_files  %s\n", toString(all_config_files).c_str());
    no_configs = all_config_files.size();

    insert_files_into_display();

    //render
    auto creator = View_Creator::get_view();
    creator->reset();
    auto gui = Gui::get_gui();
    auto frame = creator->setup_bar(&gui->data->sensors);
    auto [top, bot] = View_Creator::split_horizontal(frame, 4);

    creator->add_label("Config", top, Align::CENTER);

    auto config_frames = View_Creator::split_horizontal_arr(bot, NO_DISPLAYED_CONFIG);
    size_t idx = 0;
    for(auto& f : config_frames)
    {
        creator->add_label(displayed_data.at(idx++).data(), f, Align::CENTER, MAX_CONFIG_NAME_LEN);
    }
}

void Main_Select_Config::action_long()
{
    // Load config file
    std::string config_path = FOLDER_CONFIG_DATA + all_config_files[this->current_config_idx - 1];
    Sd_File config_file(config_path);
    std::string config_content = config_file.read_all();

    // Store the selected config path in persistent storage
    PersistentStorage storage;
    storage.set_value(Display_Actor::LAST_CONFIG_KEY, FOLDER_CONFIG_DATA + all_config_files[this->current_config_idx - 1]);

    // Send config to Data Actor
    auto payload = new Data_Actor::Sig_Data_Actor_Set_Config();
    payload->file_name = all_config_files[this->current_config_idx - 1];
    payload->file_content = config_content;
    Signal sig(actors_common::SIG_DATA_ACTOR_SET_CONFIG, payload);
    Data_Actor::get_instance().send_signal(sig);

    auto gui = Gui::get_gui();
    gui->enter();
}

void Main_Select_Config::action()
{
    if(no_configs > 0  && this->current_config_idx < no_configs)
    {
        this->current_config_idx++;
    }
    else
    {
        this->current_config_idx = 1;
    }

    insert_files_into_display();
}

void Main_Select_Config::action_second(void)
{
    if(this->current_config_idx > 1)
    {
        this->current_config_idx--;
    }
    else
    {
        this->current_config_idx = no_configs;
    }

    insert_files_into_display();
}

// #------------------------------#
// | static functions definitions |
// #------------------------------#
