// #-------------------------------#
// |           includes            |
// #-------------------------------#
// pico includes
#include "pico/time.h"
#include "pico/sem.h"

// c/c++ includes
#include <string>
#include <iostream>
#include <stdio.h>

// my includes
#include "core1.h"
#include "core_utils.hpp"
#include "common_types.h"
#include "utils.hpp"
#include "common_data.hpp"
#include "common_actors.hpp"
#include "buttons/buttons.h"
#include "traces.h"
#include "sd_file.h"
#include "session.hpp"


// display
#include "display/print.h"
#include "gui/structure.hpp"
#include "views/view.hpp"

// #-------------------------------#
// |            macros             |
// #-------------------------------#

#define SEM_TIMEOUT_MS 1000
#define FRAME_PER_SECOND 25
#define MINIMAL_TIME_BTN 300


// #------------------------------#
// | static variables definitions |
// #------------------------------#
static common_memory::Packet local_packet = {0};
static Sensor_Data& sensors_data_display = local_packet.sensors;
static Session_Data& sessionDataDisplay = local_packet.session;


static volatile bool change_state = 0;
static volatile bool stop = 0;



// #------------------------------#
// | static functions declarations|
// #------------------------------#
static void incDisplay(void);
static void incDisplayRel(void);

static void change_state_irq_handler();
static void setup(void);
static int loop(void);

static void get_total_data(float& time, float& dist);
static void set_total_data(float time, float dist);



// #------------------------------#
// | global function definitions  |
// #------------------------------#
void core1LaunchThread(void)
{
    setup();
    while (loop())
    {
    }
}

// #------------------------------#
// | static functions definitions |
// #------------------------------#

void Display_Actor::handle_sig_show_msg(const Signal &sig)
{
    auto payload = sig.get_payload<Sig_Display_Actor_Show_Msg*>();

    display::clear();
    Frame pause_label = {0, DISPLAY_HEIGHT / 4, DISPLAY_WIDTH, DISPLAY_HEIGHT / 2};
    const sFONT *font = 0;
    uint8_t scale;
    auto label = payload->msg.c_str();
    auto width_char = pause_label.width / strlen(label);
    getFontSize(width_char, pause_label.height, &font, &scale);
    Paint_Println(pause_label.x, pause_label.y, label, font, payload->color, scale);
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

void Display_Actor::handle_sig_get_file(const Signal &sig)
{
    auto payload_respond = new Sig_Data_Actor_Get_File_Respond();
    auto payload = sig.get_payload<Sig_Display_Actor_Get_File*>();
    payload_respond->type = payload->type;

    Sd_File file(payload->file_name);
    payload_respond->file_content = file.read_all();

    Signal sig_respond(SIG_DATA_ACTOR_GET_FILE_RESPOND, payload_respond);
    data_actor.send_signal(sig_respond);
    delete payload;
}

void Display_Actor::handle_sig_log(const Signal &sig)
{
    auto payload = sig.get_payload<Sig_Display_Actor_Log*>();
    Sd_File config_file(payload->file_name);
    if(config_file.is_empty())
    {
        config_file.append(payload->header.c_str());
    }
    config_file.append(payload->line.c_str());

    delete payload;
}


void Display_Actor::handle_sig_log_gps(const Signal &sig)
{
    auto payload = sig.get_payload<Sig_Display_Actor_Log_Gps*>();
    Sd_File config_file(payload->file_name);
    if(config_file.is_empty())
    {
        config_file.append("time;latitude;longitude\n");
    }
    char buffer[64] = {0};
    sprintf(buffer, "%s;%f;%f\n", time_to_str(payload->time).c_str(), payload->data.lat, payload->data.lon);
    config_file.append(buffer);

    delete payload;
}

void Display_Actor::handle_sig_total_update(const Signal &sig)
{
    float dist, time;
    // read from file dist and time
    get_total_data(time, dist);
    {
        auto payload = sig.get_payload<Sig_Display_Actor_Total_Update*>();

        // update data
        dist += payload->ridden_dist;
        time += payload->ridden_time;
        delete payload;
    }

    {
        auto payload = new Sig_Data_Actor_Set_Total();
        // update data
        payload->ridden_dist_total = dist;
        payload->ridden_time_total = time;
        Signal sig(SIG_DATA_ACTOR_SET_TOTAL, payload);
        data_actor.send_signal(sig);
    }

    // write to file
    set_total_data(time, dist);
}


static void setup(void)
{
    interruptSetupCore1();

    sensors_data_display = sensors_data;


    // setup
    auto gui = Gui::get_gui(&sensors_data_display, &sessionDataDisplay);
    gui->render();
    gui->refresh();

    {
        const std::string config_file_name = "bike_gears.cfg";
        Sd_File config_file(config_file_name);
        auto content = config_file.read_all();

        auto payload = new Sig_Data_Actor_Set_Config();
        payload->file_content = config_file.read_all();
        payload->file_name = config_file_name;
        Signal sig(SIG_DATA_ACTOR_SET_CONFIG ,payload);
        data_actor.send_signal(sig);
    }

    // update data on start
    {
        float dist = 0.0f, time = 0.0f;
        get_total_data(time, dist);
        auto payload = new Sig_Data_Actor_Set_Total();
        // update data
        payload->ridden_dist_total = dist;
        payload->ridden_time_total = time;
        Signal sig(SIG_DATA_ACTOR_SET_TOTAL, payload);
        data_actor.send_signal(sig);
    }

}

void handle_pause_start()
{
    TRACE_DEBUG(2, TRACE_CORE_1, "pause btn pressed %d\n", change_state);
    change_state = false;
    mutex_enter_blocking(&sensorDataMutex);
    switch(sensors_data_display.current_state)
    {
        case SystemState::PAUSED:
            {
                Signal sig(SIG_DATA_ACTOR_CONTINUE);
                data_actor.send_signal(sig);
                TRACE_DEBUG(2, TRACE_CORE_1, "sending signal continue %d\n", (int) sig.get_sig_id());

            }
            break;
        case SystemState::AUTOSTART:
        case SystemState::RUNNING:
            {
                Signal sig(SIG_DATA_ACTOR_PAUSE);
                data_actor.send_signal(sig);
                TRACE_DEBUG(2, TRACE_CORE_1, "sending signal pause %d\n", (int) sig.get_sig_id());

            }
            break;
        case SystemState::TURNED_ON:
            {
                Signal sig(SIG_DATA_ACTOR_SESION_START);
                data_actor.send_signal(sig);
                TRACE_DEBUG(2, TRACE_CORE_1, "sending signal start %d\n", (int) sig.get_sig_id());
            }
            break;
        default:
            TRACE_ABNORMAL(TRACE_CORE_1, "pause has no effect %d\n", (int) sensors_data_display.current_state);
            break;
    }
    sensors_data.current_state = sensors_data_display.current_state;
    mutex_exit(&sensorDataMutex);
}

void handle_end_session()
{
    display::clear();
    Frame pause_label = {0, DISPLAY_HEIGHT / 4, DISPLAY_WIDTH, DISPLAY_HEIGHT / 2};
    const sFONT *font = 0;
    uint8_t scale;
    auto label = "SAVING";
    auto width_char = pause_label.width / strlen(label);
    getFontSize(width_char, pause_label.height, &font, &scale);
    Paint_Println(pause_label.x, pause_label.y, label, font, {0x0, 0xf, 0x0}, scale);
    display::display();

    Signal sig(SIG_DATA_ACTOR_STOP);
    data_actor.send_signal(sig);

    mutex_enter_blocking(&sensorDataMutex);
    sessionDataDisplay.end(sensors_data.current_time);

    Sd_File last_save("last_track.csv");
    if (last_save.is_empty())
    {
        last_save.append(sessionDataDisplay.get_header());
        sessionDataDisplay.set_id(1);
    }
    else
    {
        auto line_no = last_save.get_no_of_lines();
        // PRINT("line_no:" << line_no);
        // for(int i = 0; i < line_no; i++)
        // {
        //     PRINT("read line " << i << ": " << last_save.read_line(i, 100));
        // }


        // extract id from last
        auto line = last_save.read_line(line_no - 2, 10); // raed only first field with id + ';'
        PRINT("last line" << line);
        auto end_pos = line.find_first_of(';');
        massert(end_pos != std::string::npos, "';' not found %s\n", line.c_str());
        auto id_str = line.substr(0, end_pos).c_str();
        uint16_t id = std::atoi(id_str) + 1;
        // massert(id != 0, "id == 0 id_str:%s\n", id_str);
        PRINT("id:" << id);
        sessionDataDisplay.set_id(id);
    }
    last_save.append(sessionDataDisplay.get_line().c_str());
    mutex_exit(&sensorDataMutex);

    // while(stop)
    {
        // PRINTF("STOPPED\n");
        display::clear();
        Frame pause_label = {0, DISPLAY_HEIGHT / 4, DISPLAY_WIDTH, DISPLAY_HEIGHT / 2};
        const sFONT *font = 0;
        uint8_t scale;
        auto label = "SAVED";
        auto width_char = pause_label.width / strlen(label);
        getFontSize(width_char, pause_label.height, &font, &scale);
        Paint_Println(pause_label.x, pause_label.y, label, font, {0x0, 0xf, 0x0}, scale);
        display::display();
        sleep_ms(1000);
    }
    // retunrn to main menu
    // Gui::get_gui()->go_back();
}

void Display_Actor::handle_sig_start_pause_btn(const Signal &sig)
{
    handle_pause_start();
}
void Display_Actor::handle_sig_end_btn(const Signal &sig)
{
    handle_end_session();
}

void Display_Actor::handle_sig_load_session(const Signal &sig)
{
    auto payload = sig.get_payload<Sig_Display_Actor_Load_Session*>();
    auto id = payload->session_id;
    delete payload;
    std::string content;
    {
        Sd_File last_save("last_track.csv");
        auto no_sessions = last_save.get_no_of_lines();
        no_sessions = no_sessions <= 2 ? no_sessions = 0 : no_sessions - 2;
        if(no_sessions < id)
        {
            return;
        }
        content = last_save.read_line(id, SESION_DATA_CSV_LEN_NO_GEARS);
    }

    // TODO send it to core 0
    Unique_Mutex mtx(&sensorDataMutex);
    if (session_p)
        delete session_p;
    session_p = new Session_Data(content.c_str(), false); // TODO load gear if needed in the future
}


static int loop(void)
{
    absolute_time_t frameStart = get_absolute_time();
    display_actor.handle_all();
    // frame update
    {
        Gui::get_gui()->handle_buttons();

        // send packet
        {
            if(sem_acquire_timeout_ms(&number_of_queueing_portions, SEM_TIMEOUT_MS))
            {
                Unique_Mutex um(&pc_mutex);
                auto queue = common_memory::get_pc_queue();
                ring_buffer_pop(queue, (char*) &local_packet);
                sem_release(&number_of_empty_positions);
            }

        }



        auto system_sate = sensors_data_display.current_state;
        // mutex_exit(&sensorDataMutex);

        // render
        auto gui = Gui::get_gui();
        gui->refresh();

        if(system_sate == SystemState::AUTOSTART)
        {
            TRACE_DEBUG(2, TRACE_CORE_1, "printing pause label %d\n", change_state);
            Frame pause_label = {0, DISPLAY_HEIGHT / 4, DISPLAY_WIDTH, DISPLAY_HEIGHT / 2};
            const sFONT* font = 0;
            uint8_t scale;
            auto label = "waiting for start";
            auto width_char = pause_label.width / strlen(label);
            getFontSize(width_char, pause_label.height, &font, &scale);
            Paint_Println(pause_label.x, pause_label.y, label, font, {0x0,0xf,0x1}, scale);
        }


        if(system_sate == SystemState::PAUSED)
        {
            TRACE_DEBUG(2, TRACE_CORE_1, "printing pause label %d\n", change_state);
            Frame pause_label = {0, DISPLAY_HEIGHT / 4, DISPLAY_WIDTH, DISPLAY_HEIGHT / 2};
            const sFONT* font = 0;
            uint8_t scale;
            auto label = "PAUSED";
            auto width_char = pause_label.width / strlen(label);
            getFontSize(width_char, pause_label.height, &font, &scale);
            Paint_Println(pause_label.x, pause_label.y, label, font, {0xf,0x0,0x0}, scale);
        }
        display::display();
    }

    absolute_time_t frameEnd = get_absolute_time();
    auto frameTimeUs = absolute_time_diff_us(frameStart, frameEnd);
    if(fpsToUs(FRAME_PER_SECOND) > frameTimeUs)
    {
        // frame took less time
        int64_t timeToSleep = fpsToUs(FRAME_PER_SECOND) - frameTimeUs;
        sleep_us(timeToSleep);
        TRACE_DEBUG(1, TRACE_CORE_1, "frame took %" PRIi64 " max time is %" PRIi64 " sleeping %" PRIi64 "\n",frameTimeUs, fpsToUs(FRAME_PER_SECOND), timeToSleep);
    }
    else
    {
        int64_t timeToSleep = fpsToUs(FRAME_PER_SECOND) - frameTimeUs;
        TRACE_ABNORMAL(TRACE_CORE_1, "frame took %" PRIi64 " should be %" PRIi64 " delta %" PRIi64 "\n",frameTimeUs, fpsToUs(FRAME_PER_SECOND), timeToSleep);
    }
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

static void get_total_data(float& time, float& dist)
{
    Sd_File total_stats("total_stats.txt");
    const auto stats = total_stats.read_all();
    const auto dist_time = split_string(stats, ';');
    dist = 0.0f, time = 0.0f;
    if(dist_time.size() == 2)
    {
        dist = std::atof(dist_time.at(0).c_str());
        time = std::atof(dist_time.at(1).c_str());
    }
}
