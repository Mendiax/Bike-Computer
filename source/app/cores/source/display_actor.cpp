// #-------------------------------#
// |           includes            |
// #-------------------------------#
// pico includes
#include "pico/time.h"
#include "pico/sem.h"
// #include "hardware/watchdog.h"

// c/c++ includes
#include <string>
#include <iostream>
#include <stdio.h>
#include <memory>

// my includes
#include "display_actor.hpp"
#include "core_utils.hpp"
#include "common_types.h"
#include "signals.hpp"
#include "utils.hpp"

#include "common_actors.hpp"
#include "display_actor.hpp"
#include "data_actor.hpp"
#include "buttons/buttons.h"
#include "traces.h"
#include "sd_file.h"
#include "session.hpp"


// display
#include "display/driver.hpp"
#include "gui/structure.hpp"
#include "views/view.hpp"



// #-------------------------------#
// |            macros             |
// #-------------------------------#

#define FRAME_PER_SECOND 25


// #------------------------------#
// | static variables definitions |
// #------------------------------#



// #------------------------------#
// | static functions declarations|
// #------------------------------#
static void get_total_data(float& time, float& dist);
static void set_total_data(float time, float dist);



// #------------------------------#
// | global function definitions  |
// #------------------------------#

void Display_Actor::run_thread(void)
{
    setup();
    while (loop())
    {
    }
}

// #------------------------------#
// | static functions definitions |
// #------------------------------#

void Display_Actor::handle_sig_get_packet(const Signal &sig)
{
    auto payload = sig.get_payload<Display_Actor::Sig_Display_Actor_Get_Packet>();

    Display_Actor::get_instance().set_local_data(*payload->packet_p);
    {
        Signal sig(actors_common::SIG_DATA_ACTOR_REQ_PACKET, payload);
        Data_Actor::get_instance().send_signal(sig);
    }
}

void Display_Actor::handle_sig_show_msg(const Signal &sig)
{
    auto payload = sig.get_payload<Display_Actor::Sig_Display_Actor_Show_Msg>();

    display::clear();
    Frame pause_label = {0, DISPLAY_HEIGHT / 4, DISPLAY_WIDTH, DISPLAY_HEIGHT / 2};
    const sFONT *font = 0;
    uint8_t scale;
    auto label = payload->msg.c_str();
    auto width_char = pause_label.width / strlen(label);
    getFontSize(width_char, pause_label.height, &font, &scale);
    display::println(pause_label.x, pause_label.y, label, font, payload->color, scale);
    display::display();
    sleep_ms(payload->time_ms);
    if(payload->time_ms == 0)
    {
        while (1) {
            TRACE_ABNORMAL(TRACE_CORE_1, "error occured '%s'\n", label);
            sleep_ms(100000);
        }
    }
    delete payload;
}

void Display_Actor::handle_sig_log(const Signal &sig)
{
    static std::unique_ptr<Sd_File> config_file_ptr;
    auto time_start = get_absolute_time();
    auto payload = sig.get_payload<Display_Actor::Sig_Display_Actor_Log>();

    auto time_start_check = get_absolute_time();
    absolute_time_t time_start_header = 0;
    absolute_time_t time_end_header = 0;
    if(!config_file_ptr ||  payload->file_name != config_file_ptr->get_file_name())
    {
        // if file name is different, create new file
        TRACE_DEBUG(3, TRACE_CORE_1, "creating new file %s\n", payload->file_name.c_str());
        config_file_ptr = std::make_unique<Sd_File>(payload->file_name);
        time_start_header = get_absolute_time();
        if(config_file_ptr->is_empty())
        {
            config_file_ptr->append(payload->header.c_str());
        }
        time_end_header = get_absolute_time();
    }
    auto time_end_check = get_absolute_time();



    auto time_start_append = get_absolute_time();
    config_file_ptr->append(payload->line.c_str(), false);
    auto time_end_append = get_absolute_time();

    auto time_start_del = get_absolute_time();
    delete payload;
    auto time_end = get_absolute_time();
    auto time_diff = absolute_time_diff_us(time_start, time_end);
    TRACE_DEBUG(3, TRACE_CORE_1, "log file written in %" PRIu64 " us"
                   " check time: %" PRIu64 " us, header time: %" PRIu64 " us, append time: %" PRIu64 " us, delete time: %" PRIu64 " us\n",
                   time_diff,
                   absolute_time_diff_us(time_start_check, time_end_check),
                   absolute_time_diff_us(time_start_header, time_end_header),
                   absolute_time_diff_us(time_start_append, time_end_append),
                   absolute_time_diff_us(time_start_del, time_end));
}


void Display_Actor::handle_sig_total_update(const Signal &sig)
{
    float dist, time;
    // read from file dist and time
    get_total_data(time, dist);
    {
        auto payload = sig.get_payload<Display_Actor::Sig_Display_Actor_Total_Update>();

        // update data
        dist += payload->ridden_dist;
        time += payload->ridden_time;
        delete payload;
    }

    {
        auto payload = new Data_Actor::Sig_Data_Actor_Set_Total();
        // update data
        payload->ridden_dist_total = dist;
        payload->ridden_time_total = time;
        Signal sig(actors_common::SIG_DATA_ACTOR_SET_TOTAL, payload);
        Data_Actor::get_instance().send_signal(sig);
    }

    // write to file
    set_total_data(time, dist);
}

#ifndef CONFIG_FILE_NAME
#define CONFIG_FILE_NAME "bike_gears.cfg"
#endif
void Display_Actor::setup(void)
{
    TRACE_DEBUG(0, TRACE_CORE_1, "interrupt setup\n");
    interruptSetupCore1();
    // setup
    Sensor_Data* sensor_data_p = &Display_Actor::get_instance().get_local_data().sensors;
    Session_Data* session_data_p = &Display_Actor::get_instance().get_local_data().session;

    TRACE_DEBUG(0, TRACE_CORE_1, "creating GUI\n");
    this->gui = Gui::get_gui(sensor_data_p, session_data_p);
    this->gui->render();
    this->gui->refresh();

    TRACE_DEBUG(0, TRACE_CORE_1, "reading config file %s\n", CONFIG_FILE_NAME);
    {
        Sd_File config_file(CONFIG_FILE_NAME);
        auto content = config_file.read_all();
        TRACE_DEBUG(0, TRACE_CORE_1, "config file content:\n%s\n", content.c_str());

        auto payload = new Data_Actor::Sig_Data_Actor_Set_Config();
        payload->file_content = config_file.read_all();
        payload->file_name = std::string(CONFIG_FILE_NAME);
        TRACE_DEBUG(0, TRACE_CORE_1, "[config payload] pointer:%p\n",payload);
        TRACE_DEBUG(0, TRACE_CORE_1, "[config payload] name:%s\n",payload->file_name.c_str());
        TRACE_DEBUG(0, TRACE_CORE_1, "[config payload] content:%s\n",payload->file_content.c_str());
        Signal sig(actors_common::SIG_DATA_ACTOR_SET_CONFIG ,payload);
        Data_Actor::get_instance().send_signal(sig);
    }

    TRACE_DEBUG(0, TRACE_CORE_1, "send total data\n");
    // update data on start
    {
        float dist = 0.0f, time = 0.0f;
        get_total_data(time, dist);
        auto payload = new Data_Actor::Sig_Data_Actor_Set_Total();
        // update data
        payload->ridden_dist_total = dist;
        payload->ridden_time_total = time;
        Signal sig(actors_common::SIG_DATA_ACTOR_SET_TOTAL, payload);
        Data_Actor::get_instance().send_signal(sig);
    }

    TRACE_DEBUG(0, TRACE_CORE_1, "send req packet\n");
    auto payload = new Data_Actor::Sig_Display_Actor_Req_Packet();
    payload->packet_p = new actors_common::Packet();
    {
        Signal sig(actors_common::SIG_DATA_ACTOR_REQ_PACKET, payload);
        Data_Actor::get_instance().send_signal(sig);
    }

}


void Display_Actor::handle_sig_save_session(const Signal &sig)
{
    auto payload = sig.get_payload<Display_Actor::Sig_Display_Actor_Save_Session>();
    auto& session_to_save = payload->session;

    display::clear();
    Frame pause_label = {0, DISPLAY_HEIGHT / 4, DISPLAY_WIDTH, DISPLAY_HEIGHT / 2};
    const sFONT *font = 0;
    uint8_t scale;
    auto label = "SAVING";
    auto width_char = pause_label.width / strlen(label);
    getFontSize(width_char, pause_label.height, &font, &scale);
    display::println(pause_label.x, pause_label.y, label, font, {0x0, 0xf, 0x0}, scale);
    display::display();
    sleep_ms(1000);

    Sd_File last_save(Display_Actor::get_session_log_file_name());
    if (last_save.is_empty())
    {
        last_save.append(session_to_save.get_header());
        session_to_save.set_id(1);
    }
    else
    {
        auto line_no = last_save.get_no_of_lines();
        // extract id from last
        auto line = last_save.read_line(line_no - 2, 10); // raed only first field with id + ';'
        auto end_pos = line.find_first_of(';');
        auto id_str = line.substr(0, end_pos).c_str();
        uint16_t id = std::atoi(id_str) + 1;

        session_to_save.set_id(id);
    }
    last_save.append(session_to_save.get_line().c_str());

    {
        // PRINTF("STOPPED\n");
        display::clear();
        Frame pause_label = {0, DISPLAY_HEIGHT / 4, DISPLAY_WIDTH, DISPLAY_HEIGHT / 2};
        const sFONT *font = 0;
        uint8_t scale;
        auto label = "SAVED";
        auto width_char = pause_label.width / strlen(label);
        getFontSize(width_char, pause_label.height, &font, &scale);
        display::println(pause_label.x, pause_label.y, label, font, {0x0, 0xf, 0x0}, scale);
        display::display();
        sleep_ms(1000);
    }

    delete payload;
}

void Display_Actor::handle_sig_load_session(const Signal &sig)
{
    auto payload = sig.get_payload<Display_Actor::Sig_Display_Actor_Load_Session>();
    auto id = payload->session_id;
    delete payload;
    std::string content;
    {
        Sd_File last_save(Display_Actor::get_session_log_file_name());
        auto no_sessions = last_save.get_no_of_lines();
        no_sessions = no_sessions <= 2 ? 0 : no_sessions - 2;
        if(no_sessions < id)
        {
            return;
        }
        content = last_save.read_line(id, SESSION_DATA_CSV_LEN);
    }

    {
        auto payload = new Data_Actor::Sig_Data_Actor_Load_Session();
        payload->session_string=std::move(content);
        Signal sig(actors_common::SIG_DATA_ACTOR_SESSION_LOAD, payload);
        Data_Actor::get_instance().send_signal(sig);
    }
}


int Display_Actor::loop(void)
{
    const absolute_time_t frameStart = get_absolute_time();
    Display_Actor::get_instance().handle_all();
    const absolute_time_t timeAferHandlers = get_absolute_time();
    // frame update
    {
        this->gui->handle_buttons();

        // render
        this->gui->refresh();

        if(Display_Actor::get_instance().get_local_data().session.get_status() == Session_Data::Status::PAUSED)
        {
            TRACE_DEBUG(2, TRACE_CORE_1, "printing pause label\n");
            Frame pause_label = {DISPLAY_WIDTH / 10, DISPLAY_HEIGHT / 3, DISPLAY_WIDTH, DISPLAY_HEIGHT / 4};
            const sFONT* font = 0;
            uint8_t scale;
            auto label = "PAUSED";
            auto width_char = pause_label.width / strlen(label);
            getFontSize(width_char, pause_label.height, &font, &scale);
            display::println(pause_label.x, pause_label.y, label, font, {0xf,0x0,0x0}, scale);
            // draw red frame
            const uint8_t frame_scale = 3;
            display::draw_line(0, 0, DISPLAY_WIDTH - 1, 0, {0xf, 0x0, 0x0}, frame_scale);
            display::draw_line(0, DISPLAY_HEIGHT - 1, DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 1, {0xf, 0x0, 0x0}, frame_scale);

            display::draw_line(0, 0, 0, DISPLAY_HEIGHT - 1, {0xf, 0x0, 0x0}, frame_scale);
            display::draw_line(DISPLAY_WIDTH - 1, 0, DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 1, {0xf, 0x0, 0x0}, frame_scale);

        }
        display::display();
    }
    const absolute_time_t frameEnd = get_absolute_time();

    const auto handlersTime = absolute_time_diff_us(frameStart, timeAferHandlers);
    const auto displayTime = absolute_time_diff_us(timeAferHandlers, frameEnd);
    const auto frameTimeUs = absolute_time_diff_us(frameStart, frameEnd);

    const int64_t timeToSleep = fpsToUs(FRAME_PER_SECOND) - frameTimeUs;
    TRACE_DEBUG(1, TRACE_CORE_1, "frame took %" PRIi64 " [handlers: %" PRIi64 ", render: %" PRIi64 " ] should be %" PRIi64 " delta %" PRIi64 "\n",
        frameTimeUs, handlersTime, displayTime, fpsToUs(FRAME_PER_SECOND), timeToSleep);
    sleep_us(std::max(timeToSleep, (int64_t)0));

    return 1;
}

static void set_total_data(float time, float dist)
{
    Sd_File total_stats("total_stats.txt");
    const auto stats = total_stats.read_all();
    std::string new_dist_time = std::to_string(dist) + ';' + std::to_string(time);
    if(new_dist_time.length() < stats.length())
    {
        total_stats.clear();
    }
    total_stats.overwrite(new_dist_time.c_str());
}
#ifndef FILE_TOTAL_DATA
#define FILE_TOTAL_DATA "total_stats.txt"
#endif

static void get_total_data(float& time, float& dist)
{
    Sd_File total_stats(FILE_TOTAL_DATA);
    const auto stats = total_stats.read_all();
    const auto dist_time = split_string(stats, ';');
    dist = 0.0f, time = 0.0f;
    if(dist_time.size() == 2)
    {
        dist = std::atof(dist_time.at(0).c_str());
        time = std::atof(dist_time.at(1).c_str());
    }
}
