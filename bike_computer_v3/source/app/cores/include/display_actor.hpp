#ifndef CORE1_H
#define CORE1_H

#include <pico/sync.h>
#include "common_types.h"
#include "common_actors.hpp"

#include "session.hpp"
#include "common_types.h"
#include "common_data.hpp"
#include "gui/structure.hpp"

class Display_Actor : public Actor
{
public:
    struct Sig_Display_Actor_Show_Msg {
        std::string msg;
        display::DisplayColor color;
        uint time_ms;
    };

    struct Sig_Display_Actor_Log {
        std::string header;
        std::string line;
        std::string file_name;
    };

    // struct Sig_Display_Actor_Log_Gps {
    //     GpsDataS data;
    //     Time_HourS time;
    //     std::string file_name;
    // };

    struct Sig_Display_Actor_Get_Packet {
        actors_common::Packet* packet_p;
    };

    struct Sig_Display_Actor_End_Sesion {
        bool save;
    };

    struct Sig_Display_Actor_Save_Sesion {
        Session_Data session;
    };

    struct Sig_Display_Actor_Load_Session {
        uint16_t session_id;
    };

    struct Sig_Display_Actor_Total_Update {
        float ridden_dist;
        float ridden_time;
    };

private:
    Gui* gui;
    actors_common::Packet local_data;

    // definde in core1.cpp
    static void handle_sig_total_update(const Signal &sig);
    // static void handle_sig_start_pause_btn(const Signal &sig);
    // static void handle_sig_end_btn(const Signal &sig);
    static void handle_sig_load_session(const Signal &sig);
    static void handle_sig_save_session(const Signal &sig);

    // static void handle_sig_log_gps(const Signal &sig);
    static void handle_sig_log(const Signal &sig);
    static void handle_sig_show_msg(const Signal &sig);

    static void handle_sig_get_packet(const Signal &sig);


    void handler_setup()
    {
        this->handler_add(handle_sig_total_update, actors_common::SIG_DISPLAY_ACTOR_TOTAL_UPDATE);
        // this->handler_add(handle_sig_start_pause_btn, actors_common::SIG_DISPLAY_ACTOR_START_PAUSE_BTN);
        // this->handler_add(handle_sig_end_btn, actors_common::SIG_DISPLAY_ACTOR_END_BTN);
        this->handler_add(handle_sig_load_session, actors_common::SIG_DISPLAY_ACTOR_LOAD_SESSION);
        this->handler_add(handle_sig_save_session, actors_common::SIG_DISPLAY_ACTOR_SAVE_SESSION);


        // this->handler_add(handle_sig_log_gps, actors_common::SIG_DISPLAY_ACTOR_LOG_GPS);
        this->handler_add(handle_sig_log, actors_common::SIG_DISPLAY_ACTOR_LOG);
        this->handler_add(handle_sig_show_msg, actors_common::SIG_DISPLAY_ACTOR_SHOW_MSG);
        this->handler_add(handle_sig_get_packet, actors_common::SIG_DISPLAY_ACTOR_GET_PACKET);


        // this->handler_add(handle_sig_get_file, actors_common::SIG_DISPLAY_ACTOR_GET_FILE);
    }

    void setup();
    int loop();


    void run_thread(void);
    Display_Actor()
    {
        handler_setup();
        local_data = {0};
    }
    friend void core1LaunchThread(void);
public:
    inline actors_common::Packet& get_local_data() {return local_data;}
    inline void set_local_data(const actors_common::Packet& data) {
        this->local_data = data;
    }

    static inline const char* get_session_log_file_name(){
        return "session_log.csv";
    }
    static inline Display_Actor& get_instance()
    {
        static Display_Actor* this_p = nullptr;
        if(this_p == nullptr)
        {
            this_p = new Display_Actor();
        }

        return *this_p;
    }
};


#endif