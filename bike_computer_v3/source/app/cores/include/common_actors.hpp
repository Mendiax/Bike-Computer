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
    SIG_CORE0_NOT_EXIST,
    SIG_CORE0_SESION_START,
    SIG_CORE0_PAUSE,
    SIG_CORE0_START,
    SIG_CORE0_STOP,
    SIG_CORE0_CONTINUE,
    SIG_CORE0_SET_CONFIG,
    SIG_CORE0_SET_TOTAL,
    SIG_CORE0_GET_FILE_RESPOND,

    // core 1
    SIG_CORE1_TOTAL_UPDATE,
    SIG_CORE1_START_PAUSE_BTN,
    SIG_CORE1_END_BTN,
    SIG_CORE1_LOAD_SESSION,
    SIG_CORE1_LOG_GPS,
    SIG_CORE1_LOG,
    SIG_CORE1_GET_FILE,
    SIG_CORE1_SHOW_MSG,
    SIG_NO_MAX
};

enum class File_Respond{
    time_offset
};

struct Sig_Core0_Get_File_Respond{
    File_Respond type;
    std::string file_content;
};

struct Sig_Core1_Get_File{
    File_Respond type;
    std::string file_name;
};

struct Sig_Core1_Show_Msg{
    std::string msg;
    display::DisplayColor color;
    uint time_ms;
};

struct Sig_Core1_Log{
    std::string header;
    std::string line;
    std::string file_name;
};

struct Sig_Core1_Log_Gps{
    GpsDataS data;
    Time_HourS time;
    std::string file_name;
};

struct Sig_Core1_Load_Session{
    uint16_t session_id;
};

struct Sig_Core1_Total_Update{
    float ridden_dist;
    float ridden_time;
};

struct Sig_Core0_Set_Config{
    std::string file_name;
    std::string file_content;
};

struct Sig_Core0_Set_Total{
    float ridden_dist_total;
    float ridden_time_total;
};

class Core0 : public Actor
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
        this->handler_add(handle_sig_pause, SIG_CORE0_PAUSE);
        this->handler_add(handle_sig_continue, SIG_CORE0_CONTINUE);
        this->handler_add(handle_sig_start, SIG_CORE0_START);
        this->handler_add(handle_sig_stop, SIG_CORE0_STOP);
        this->handler_add(handle_sig_session_start, SIG_CORE0_SESION_START);
        this->handler_add(handle_sig_set_config, SIG_CORE0_SET_CONFIG);
        this->handler_add(handle_sig_set_total, SIG_CORE0_SET_TOTAL);
        this->handler_add(handle_sig_get_file_resond, SIG_CORE0_GET_FILE_RESPOND);
    }

public:
    Core0()
    {
        handler_setup();
    }
};

class Core1 : public Actor
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
        this->handler_add(handle_sig_total_update, SIG_CORE1_TOTAL_UPDATE);
        this->handler_add(handle_sig_start_pause_btn, SIG_CORE1_START_PAUSE_BTN);
        this->handler_add(handle_sig_end_btn, SIG_CORE1_END_BTN);
        this->handler_add(handle_sig_load_session, SIG_CORE1_LOAD_SESSION);
        this->handler_add(handle_sig_log_gps, SIG_CORE1_LOG_GPS);
        this->handler_add(handle_sig_log, SIG_CORE1_LOG);
        this->handler_add(handle_sig_show_msg, SIG_CORE1_SHOW_MSG);
        this->handler_add(handle_sig_get_file, SIG_CORE1_GET_FILE);

        // this->handler_add(handle_sig_get_file, SIG_CORE1_GET_FILE);
    }

public:
    Core1()
    {
        handler_setup();
    }
};

// #-------------------------------#
// | global variables declarations |
// #-------------------------------#

extern Core0 actor_core0;
extern Core1 actor_core1;



// #-------------------------------#
// | global function declarations  |
// #-------------------------------#

#endif
