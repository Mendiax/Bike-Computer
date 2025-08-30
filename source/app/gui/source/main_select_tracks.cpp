
// #------------------------------#
// |          includes            |
// #------------------------------#
// pico includes

// c/c++ includes
#include <string.h>

// my includes
#include "gui/main/main_select_tracks.hpp"
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
Main_Select_Tracks::Main_Select_Tracks(gui::View_List* p)
    :gui::View(p)
{
    // sessions = new Sd_File(Display_Actor::get_session_log_file_name());
    // no_sessions = sessions->get_no_of_lines() - 1;
    // TRACE_DEBUG(2, TRACE_GUI, "no_sessions = %zu\n", no_sessions);
    // TRACE_DEBUG(2, TRACE_GUI, "last_track create\n");

}

void Main_Select_Tracks::insert_files_into_display(void)
{
    TRACE_DEBUG(2, TRACE_GUI, "no_sessions = %zu\n", no_tracks);

    size_t data_idx = this->current_log_idx;
    for(auto& f : this->displayed_data)
    {
        if(data_idx <= no_tracks)
        {
            auto track_name = all_tracks_files.at(data_idx - 1);
            strncpy(f.data(), track_name.c_str(), MAX_TRACK_NAME_LEN);
            f[MAX_TRACK_NAME_LEN] = '\0';
            TRACE_DEBUG(2, TRACE_GUI, "id:%zu, track:%s\n", data_idx, f.data());
            data_idx++;
        }
        else
        {
            strcpy(f.data(), "");
        }
    }

    for(size_t idx = 0; idx < NO_DISPLAYED_HISTORY; idx++)
    {
        TRACE_DEBUG(2, TRACE_GUI, "Data[%zu] %s\n", idx, displayed_data.at(idx).data());
    }
}


#ifndef FOLDER_TRACKS_DATA
#define FOLDER_TRACKS_DATA "tracks/"
#endif

void Main_Select_Tracks::render()
{
    this->current_log_idx = 1;

    // insert data into displayed_data
    all_tracks_files = dir::get_files(FOLDER_TRACKS_DATA);
    TRACE_DEBUG(2, TRACE_GUI, "all_tracks_files  %s\n", toString(all_tracks_files).c_str());
    no_tracks = all_tracks_files.size();

    insert_files_into_display();

    //render
    auto creator = View_Creator::get_view();
    creator->reset();
    auto gui = Gui::get_gui();
    auto frame = creator->setup_bar(&gui->data->sensors);
    auto [top, bot] = View_Creator::split_horizontal(frame, 4);

    creator->add_label("Tracks", top, Align::CENTER);

    auto hist_frames = View_Creator::split_horizontal_arr(bot, NO_DISPLAYED_HISTORY);
    size_t idx = 0;
    for(auto& f : hist_frames)
    {
        creator->add_label(displayed_data.at(idx++).data(), f, Align::CENTER, MAX_TRACK_NAME_LEN);
    }
    creator->add_frame(hist_frames.at(0));
}
void Main_Select_Tracks::action_long()
{
    auto payload = new Display_Actor::Sig_Display_Actor_Load_Track();
    payload->track_path = FOLDER_TRACKS_DATA + all_tracks_files[this->current_log_idx - 1];
    Signal sig(actors_common::SIG_DISPLAY_ACTOR_LOAD_TRACK, payload);
    Display_Actor::get_instance().send_signal(sig);

    auto gui = Gui::get_gui();
    gui->enter();
    Data_Actor::get_instance().send_signal(Signal(actors_common::SIG_DATA_ACTOR_SESSION_START));

    // PRINTF("main manu new session btn pressed next list:%p\n", this->get_next_view_list());
}


void Main_Select_Tracks::action()
{
    if(no_tracks > 0  && this->current_log_idx < no_tracks)
    {
        this->current_log_idx++;
    }
    else
    {
        this->current_log_idx = 1;
    }

    insert_files_into_display();
}

void  Main_Select_Tracks::action_second(void)
{
    if(this->current_log_idx > 1)
    {
        this->current_log_idx--;
    }
    else
    {
        this->current_log_idx = no_tracks;
    }

    insert_files_into_display();
}

// #------------------------------#
// | static functions definitions |
// #------------------------------#
