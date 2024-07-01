// #-------------------------------#
// |           includes            |
// #-------------------------------#
// pico includes
#include <cstddef>
#include <cstdint>
#include <pico/stdlib.h>
#include "display/driver.hpp"
#include "pico/time.h"
#include "pico/util/datetime.h"

// c/c++ includes
#include <stdio.h>
#include <tuple>
#include <inttypes.h>
#include <string.h>
#include <iostream>
#include <sstream>

// my includes
#include "core_utils.hpp"
#include "data_actor.hpp"
#include "ringbuffer.h"
#include "session.hpp"
#include "signals.hpp"
#include "traces.h"
#include "common_types.h"
#include "dof.hpp"
#include "bmp280.hpp"
#include "MPU9250.hpp"

#include "speedometer/speedometer.hpp"
#include "cadence/cadence.hpp"
#include "interrupts/interrupts.hpp"

#include "common_actors.hpp"
#include "display_actor.hpp"
#include "data_actor.hpp"
#include "sd_file.h"
// SIM868
#include "sim868/interface.hpp"
#include "sim868/gps.hpp"

#include "hardware/rtc.h"
#include "utils.hpp"

// #-------------------------------#
// |            macros             |
// #-------------------------------#
#define DATA_PER_SECOND 25

#define SEM_TIMEOUT_MS 1000

// cycles times
#define BAT_LEV_CYCLE_MS (29*1000)
#define WEATHER_CYCLE_MS (100)
#define HEART_BEAT_CYCLE_MS (10*1000)
#define GPS_FETCH_CYCLE_MS (1*1000)
#define TIME_FETCH_CYCLE_MS (10*1000)
// http requests per 10min
#define GSM_FETCH_CYCLE_MS (10*60*1000)

#define LOG_DATA_CYCLE_MS (1 * 1000)

// #define SIM_SPEED
// #define SIM_SLOW_DOWN

//switches
#define SIM_WHEEL_CADENCE 1
#define PREDEFINED_BIKE_SETUP 0


// #------------------------------#
// | global variables definitions |
// #------------------------------#


// #------------------------------#
// | static variables definitions |
// #------------------------------#
extern Bike_Config config;
extern bool config_received;
extern Sensor_Data sensors_data;
extern Session_Data *session_p;


// #------------------------------#
// | static functions declarations|
// #------------------------------#
static bool load_config_from_file(const char* config_str, const char* file_name);



// #------------------------------#
// | global function definitions  |
// #------------------------------#

void Data_Actor::handle_sig_set_config(const Signal &sig)
{
    const auto payload = sig.get_payload<Data_Actor::Sig_Data_Actor_Set_Config*>();
    config_received = load_config_from_file(payload->file_content.c_str(), payload->file_name.c_str());
    if(!config_received)
    {
        // send msg to user

        auto payload = new Display_Actor::Sig_Display_Actor_Show_Msg();
        *payload = {
            std::string("Cannot load bike config"),
            {0xf,0xf,0xf},
            0
        };
        Signal sig(actors_common::SIG_DISPLAY_ACTOR_SHOW_MSG, payload);
        Display_Actor::get_instance().send_signal(sig);
    }
    else
    {
        config.to_string();
    }
    delete payload;
}

void Data_Actor::handle_sig_session_load(const Signal &sig)
{
    auto payload = sig.get_payload<Data_Actor::Sig_Data_Actor_Load_Session*>();

    if (session_p)
        delete session_p;
    session_p = new Session_Data(payload->session_string.c_str());
    delete payload;
}

void Data_Actor::handle_sig_set_total(const Signal &sig)
{
    const auto payload = sig.get_payload<Data_Actor::Sig_Data_Actor_Set_Total*>();

    sensors_data.total_time_ridden = payload->ridden_time_total;
    sensors_data.total_distance_ridden = payload->ridden_dist_total;
    delete payload;
}

void Data_Actor::handle_sig_req_packet(const Signal &sig)
{
    auto payload = sig.get_payload<Data_Actor::Sig_Display_Actor_Req_Packet*>();
    if(session_p)
    {
        *payload->packet_p = {sensors_data, *session_p};
        {
            Signal sig(actors_common::SIG_DISPLAY_ACTOR_GET_PACKET, payload);
            Display_Actor::get_instance().send_signal(sig);
        }
    }
    else {
        *payload->packet_p = {sensors_data, Session_Data()};
        {
            Signal sig(actors_common::SIG_DISPLAY_ACTOR_GET_PACKET, payload);
            Display_Actor::get_instance().send_signal(sig);
        }
    }
}



void Data_Actor::handle_sig_pause([[maybe_unused]] const Signal &sig)
{
    speed::stop();
    session_p->pause();
}
void Data_Actor::handle_sig_continue([[maybe_unused]] const Signal &sig)
{
    session_p->cont();
    speed::start();
}


void Data_Actor::handle_sig_session_start([[maybe_unused]] const Signal &sig)
{
    speed::stop();
    speed::reset();
    {
        if(session_p != nullptr)
            delete session_p;
        session_p = new Session_Data();
        session_p->start(sensors_data.current_time);
        session_p->pause();
    }
}

void Data_Actor::handle_sig_start([[maybe_unused]] const Signal &sig)
{
    session_p->pause();
    speed::start(); // TODO popraw start

}
void Data_Actor::handle_sig_stop(const Signal &sig)
{
    speed::stop();

    // sensors_data.current_state = SystemState::TURNED_ON;

    if(session_p == nullptr)
    {
        return;
    }

    auto payload = sig.get_payload<Display_Actor::Sig_Display_Actor_End_Sesion*>();

    if(session_p->has_started())
    {
        session_p->end(sensors_data.current_time);
        if(payload->save)
        {
            auto payload = new Display_Actor::Sig_Display_Actor_Save_Sesion();
            payload->session = *session_p;
            Signal sig(actors_common::SIG_DISPLAY_ACTOR_SAVE_SESSION, payload);
            Display_Actor::get_instance().send_signal(sig);
        }
    }
    if(session_p != nullptr)
        delete session_p;
    session_p = new Session_Data();
}

static bool load_config_from_file(const char* config_str, const char* file_name)
{
    bool success = config.from_string(config_str);
    auto name = split_string(file_name, '.');
    if(name.size() > 0)
        config.name = name.at(0);
    else
        config.name = "unknown";
    cadence::setup(config.no_magnets_cadence);
    speed::setup(config.wheel_size, config.no_magnets_speed);

    return success;
}