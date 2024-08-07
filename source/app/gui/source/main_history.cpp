
// #------------------------------#
// |          includes            |
// #------------------------------#
// pico includes

// c/c++ includes
#include <string.h>

// my includes
#include "gui/main/main_history.hpp"
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
Main_History::Main_History(gui::View_List* p)
    :gui::View(p)
{
    // sessions = new Sd_File(Display_Actor::get_session_log_file_name());
    // no_sessions = sessions->get_no_of_lines() - 1;
    // TRACE_DEBUG(1, TRACE_GUI, "no_sessions = %zu\n", no_sessions);
    // TRACE_DEBUG(1, TRACE_GUI, "last_track create\n");

}

void extract_session_name(char* f, Sd_File& sessions, size_t id)
{
    auto line = sessions.read_line(id, 20);
    TRACE_DEBUG(1, TRACE_GUI, "id:%zu, session:%s\n", id, line.c_str());

    auto line_split = split_string(line, ';');
    auto str_id = line_split.at(0);
    auto str_date = line_split.at(1);

    auto draw_line = str_id + ")" + str_date.substr(5, 11); // len >= 13

    const size_t copy_size = std::min(draw_line.length(), (size_t)MAX_LOG_NAME_LEN);
    memcpy(f, draw_line.c_str(), copy_size);
    f[copy_size] = '\0';

    // return line;
    // Session_Data data(line.c_str(), false);
}

void Main_History::insert_files_into_display(void)
{
    // TODO reduce reads from file
    Sd_File sessions(Display_Actor::get_session_log_file_name());
    no_sessions = sessions.get_no_of_lines();
    no_sessions = (no_sessions <= 2) ? (0) : (no_sessions - 2);
    TRACE_DEBUG(1, TRACE_GUI, "no_sessions = %zu\n", no_sessions);

    size_t data_idx = this->current_log_idx;
    for(auto& f : this->displayed_data)
    {
        if(data_idx <= no_sessions)
        {
            extract_session_name(f, sessions, data_idx);
            data_idx++;
        }
        else
        {
            strcpy(f, "");
        }
    }

    for(size_t idx = 0; idx < NO_DISPLAYED_HISTORY; idx++)
    {
        TRACE_DEBUG(1, TRACE_GUI, "Data[%zu] %s\n", idx, displayed_data.at(idx));
    }
}


void Main_History::render()
{
    // get data
    // Sd_File file(Display_Actor::get_session_log_file_name());
    // no_sessions = sessions->get_no_of_lines() - 1;
    // TRACE_DEBUG(1, TRACE_GUI, "no_sessions = %zu\n", no_sessions);

    // this->all_log_files = dir::get_files("history");
    // unmount_drive();

    // auto all_sessions = sessions.read_all_lines(Session_DATA_CSV_LEN_NO_GEARS);
    // this->all_log_files.reserve(no_sessions);
    this->current_log_idx = 1;

    // insert data into displayed_data
    insert_files_into_display();

    //render
    auto creator = View_Creator::get_view();
    creator->reset();
    auto gui = Gui::get_gui();
    auto frame = creator->setup_bar(&gui->data->current_time.hours, &gui->data->lipo);
    auto [top, bot] = View_Creator::split_horizontal(frame, 4);

    creator->add_label("History", top, Align::CENTER);

    auto hist_frames = View_Creator::split_horizontal_arr(bot, NO_DISPLAYED_HISTORY);
    size_t idx = 0;
    for(auto& f : hist_frames)
    {
        creator->add_label(displayed_data.at(idx++), f, Align::CENTER, MAX_LOG_NAME_LEN);
    }
    creator->add_frame(hist_frames.at(0));
    // auto window = creator->get_previous_window();
    // labelSettingsAlignHeight(window->settings.label.text, top, false);


}
void Main_History::action_long()
{
    auto payload = new Display_Actor::Sig_Display_Actor_Load_Session();
    payload->session_id = this->current_log_idx;
    Signal sig(actors_common::SIG_DISPLAY_ACTOR_LOAD_SESSION, payload);
    Display_Actor::get_instance().send_signal(sig);

    // PRINTF("main manu new session btn pressed next list:%p\n", this->get_next_view_list());
    auto gui = Gui::get_gui();
    gui->enter();
}


void Main_History::action()
{
    if(no_sessions > 0  && this->current_log_idx < no_sessions)
    {
        this->current_log_idx++;
    }
    else
    {
        this->current_log_idx = 1;
    }

    insert_files_into_display();
}

void  Main_History::action_second(void)
{
    if(this->current_log_idx > 1)
    {
        this->current_log_idx--;
    }
    else
    {
        this->current_log_idx = no_sessions;
    }

    insert_files_into_display();
}

// #------------------------------#
// | static functions definitions |
// #------------------------------#
