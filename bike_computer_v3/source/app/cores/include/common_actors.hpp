#ifndef COMMON_ACTORS_HPP
#define COMMON_ACTORS_HPP
// #-------------------------------#
// |           includes            |
// #-------------------------------#
// pico includes

// c/c++ includes
#include <cstdint>
#include <string>

// my includes
#include "common_types.h"
#include "actors.hpp"
#include "display/driver.hpp"

// #-------------------------------#
// |            macros             |
// #-------------------------------#

// #-------------------------------#
// | global types declarations     |
// #-------------------------------#
enum Sig_Ids
{
    // core 0
    SIG_DATA_ACTOR_NOT_EXIST,
    SIG_DATA_ACTOR_SESION_START,
    SIG_DATA_ACTOR_PAUSE,
    SIG_DATA_ACTOR_START,
    SIG_DATA_ACTOR_STOP,
    SIG_DATA_ACTOR_CONTINUE,
    SIG_DATA_ACTOR_SET_CONFIG,
    SIG_DATA_ACTOR_SET_TOTAL,
    SIG_DATA_ACTOR_GET_FILE_RESPOND,

    // core 1
    SIG_DISPLAY_ACTOR_TOTAL_UPDATE,
    SIG_DISPLAY_ACTOR_START_PAUSE_BTN,
    SIG_DISPLAY_ACTOR_END_BTN,
    SIG_DISPLAY_ACTOR_LOAD_SESSION,
    SIG_DISPLAY_ACTOR_LOG_GPS,
    SIG_DISPLAY_ACTOR_LOG,
    SIG_DISPLAY_ACTOR_GET_FILE,
    SIG_DISPLAY_ACTOR_SHOW_MSG,
    SIG_NO_MAX
};

enum class File_Respond{
    time_offset
};

struct Sig_Data_Actor_Get_File_Respond{
    File_Respond type;
    std::string file_content;
};

struct Sig_Display_Actor_Get_File{
    File_Respond type;
    std::string file_name;
};

struct Sig_Display_Actor_Show_Msg{
    std::string msg;
    display::DisplayColor color;
    uint time_ms;
};

struct Sig_Display_Actor_Log{
    std::string header;
    std::string line;
    std::string file_name;
};

struct Sig_Display_Actor_Log_Gps{
    GpsDataS data;
    Time_HourS time;
    std::string file_name;
};

struct Sig_Display_Actor_Load_Session{
    uint16_t session_id;
};

struct Sig_Display_Actor_Total_Update{
    float ridden_dist;
    float ridden_time;
};

struct Sig_Data_Actor_Set_Config{
    std::string file_name;
    std::string file_content;
};

struct Sig_Data_Actor_Set_Total{
    float ridden_dist_total;
    float ridden_time_total;
};

class Data_Actor : public Actor
{
private:
    // definde in core0.cpp
    static void handle_sig_session_start(const Signal &sig);
    static void handle_sig_pause(const Signal &sig);
    static void handle_sig_start(const Signal &sig);
    static void handle_sig_stop(const Signal &sig);
    static void handle_sig_continue(const Signal &sig);

    static void handle_sig_set_config(const Signal &sig);
    static void handle_sig_set_total(const Signal &sig);
    static void handle_sig_get_file_resond(const Signal &sig);

    void handler_setup()
    {
        this->handler_add(handle_sig_pause, SIG_DATA_ACTOR_PAUSE);
        this->handler_add(handle_sig_continue, SIG_DATA_ACTOR_CONTINUE);
        this->handler_add(handle_sig_start, SIG_DATA_ACTOR_START);
        this->handler_add(handle_sig_stop, SIG_DATA_ACTOR_STOP);
        this->handler_add(handle_sig_session_start, SIG_DATA_ACTOR_SESION_START);
        this->handler_add(handle_sig_set_config, SIG_DATA_ACTOR_SET_CONFIG);
        this->handler_add(handle_sig_set_total, SIG_DATA_ACTOR_SET_TOTAL);
        this->handler_add(handle_sig_get_file_resond, SIG_DATA_ACTOR_GET_FILE_RESPOND);
    }

public:
    Data_Actor()
    {
        handler_setup();
    }
};

class Display_Actor : public Actor
{
private:
    // definde in core1.cpp
    static void handle_sig_total_update(const Signal &sig);
    static void handle_sig_start_pause_btn(const Signal &sig);
    static void handle_sig_end_btn(const Signal &sig);
    static void handle_sig_load_session(const Signal &sig);
    static void handle_sig_log_gps(const Signal &sig);
    static void handle_sig_log(const Signal &sig);
    static void handle_sig_show_msg(const Signal &sig);
    static void handle_sig_get_file(const Signal &sig);




    // static void handle_sig_get_file(const Signal &sig);


    void handler_setup()
    {
        this->handler_add(handle_sig_total_update, SIG_DISPLAY_ACTOR_TOTAL_UPDATE);
        this->handler_add(handle_sig_start_pause_btn, SIG_DISPLAY_ACTOR_START_PAUSE_BTN);
        this->handler_add(handle_sig_end_btn, SIG_DISPLAY_ACTOR_END_BTN);
        this->handler_add(handle_sig_load_session, SIG_DISPLAY_ACTOR_LOAD_SESSION);
        this->handler_add(handle_sig_log_gps, SIG_DISPLAY_ACTOR_LOG_GPS);
        this->handler_add(handle_sig_log, SIG_DISPLAY_ACTOR_LOG);
        this->handler_add(handle_sig_show_msg, SIG_DISPLAY_ACTOR_SHOW_MSG);
        this->handler_add(handle_sig_get_file, SIG_DISPLAY_ACTOR_GET_FILE);

        // this->handler_add(handle_sig_get_file, SIG_DISPLAY_ACTOR_GET_FILE);
    }

public:
    Display_Actor()
    {
        handler_setup();
    }
};

// #-------------------------------#
// | global variables declarations |
// #-------------------------------#

extern Data_Actor data_actor;
extern Display_Actor display_actor;



// #-------------------------------#
// | global function declarations  |
// #-------------------------------#

#endif
