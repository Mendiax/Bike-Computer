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
static Bike_Config config;
static bool config_received = false;
static Sensor_Data sensors_data;
static Session_Data *session_p = 0;


// #------------------------------#
// | static functions declarations|
// #------------------------------#
static void cycle_get_slope();
static float calc_slope(const float distance, const float altitude);
static bool load_config_from_file(const char* config_str, const char* file_name);

static void update_total_stats();

static void cycle_log_data();
static void cycle_print_heart_beat();
static void cycle_get_battery_status();
static void cycle_get_gps_data();
static void cycle_get_weather_data();

// #------------------------------#
// | global function definitions  |
// #------------------------------#
void Data_Actor::run_thread(void)
{
    setup();
    PRINT("SETUP DONE");
    while (loop())
    {
        tight_loop_contents();
    }
}

static void gear_update(Gear_Suggestion_Calculator* gear_suggestion_calc);
static float speed_cadence_update();

#ifdef SIM_SPEED
static void set_speed_gear(float emulated_speed, uint8_t gear);

static void set_speed_gear(float emulated_speed, uint8_t gear)
{
    // float emulated_speed = 20.0;
CALC_CAD:
    float ratio = config.get_gear_ratio({1,gear});
    float wheel_rpm = speed::kph_to_rpm(emulated_speed);
    float cadence = wheel_rpm / ratio;
    if(cadence > 120 && emulated_speed > 3.0){
        emulated_speed -= 1.0;
        goto CALC_CAD;
    }

    PRINTF("emulated_speed=%f\n", emulated_speed);
    PRINTF("wheel_rpm=%f\n",wheel_rpm);
    PRINTF("ratio=%f\n",ratio);
    PRINTF("cadence=%f\n",cadence);

    speed_emulate(emulated_speed);
    cadence::emulate(cadence);
}
#endif

[[maybe_unused]]
static void send_speed_comp(float raw, float filtered)
{
    auto payload = new Display_Actor::Sig_Display_Actor_Log();
    std::stringstream ss;
    ss << "speed_comp" << time_to_str_file_name_conv(session_p->get_start_time()) << ".csv";
    payload->file_name = ss.str();
    payload->header = "velocity_raw;velocity_filtered\n";

    char buffer[100] = {0};
    sprintf(buffer, "%f;%f\n", raw, filtered);
    payload->line = buffer;

    Signal sig(actors_common::SIG_DISPLAY_ACTOR_LOG, payload);
    Display_Actor::get_instance().send_signal(sig);
}

// #------------------------------#
// | static functions definitions |
// #------------------------------#
void Data_Actor::setup(void)
{
    sensors_data.imu.accel_hist.set_length(100);

    TRACE_DEBUG(0, TRACE_MAIN, "interrupt setup core 0\n");
    interruptSetupCore0();
    TRACE_DEBUG(0, TRACE_MAIN, "INIT SIM868\n");
    // setup sim868
    sim868::init();
    sim868::turn_on();

    session_p = new Session_Data();

    rtc_init();
    datetime_t t = {
        .year  = 2000,
        .month = 01,
        .day   = 01,
        .dotw  = 6, // 0 is Sunday
        .hour  = 0,
        .min   = 0,
        .sec   = 00
    };
    rtc_set_datetime(&t);
    TRACE_DEBUG(0, TRACE_MAIN, "WAITING FOR CONFIG\n");
    // wait for config
    while(!config_received)
    {
        Data_Actor::get_instance().handle_all();
        sleep_ms(10);
    }
    TRACE_DEBUG(0, TRACE_MAIN, "CONFIG RECIVED\n");
    config.to_string();

    // for testing purpose
    #ifdef SIM_SPEED
    TRACE_DEBUG(0, TRACE_MAIN, "EMULATING SPEED\n");
    set_speed_gear(20, 8);
    #endif

    gear_suggestion_calc = new Gear_Suggestion_Calculator(config);

    //turn of power led
    gpio_init(25); //power led
    gpio_set_dir(25, GPIO_OUT);
    gpio_put(25, 0);




    TRACE_DEBUG(0, TRACE_MAIN, "INIT DOF\n");
    dof::init(true);
    mpu9250::set_reference(
        sensors_data.imu.rotation, sensors_data.imu.rotation_speed,
        sensors_data.imu.accelerometer, sensors_data.imu.magnetometer);
    TRACE_DEBUG(0, TRACE_MAIN, "DOF IS ON\n");

    TRACE_DEBUG(0, TRACE_MAIN, "WAITING FOR SIM868\n");
    // while (sim868::check_for_boot_long() == false) {
    //     sleep_ms(1000);
    // }
    // sim868::waitForBoot();
    // TRACE_DEBUG(0, TRACE_MAIN, "SIM868 IS ON\n");

    // TRACE_DEBUG(0, TRACE_MAIN, "GET BATTERY\n");
    // {
    //     bool is_charging = 0;
    //     uint8_t bat_lev = 0;
    //     uint16_t voltage = 0;
    //     while(sim868::get_bat_level(is_charging, bat_lev, voltage) == false)
    //     {
    //         sleep_ms(500);
    //     }
    //     sensors_data.lipo.is_charging = is_charging;
    //     sensors_data.lipo.level = bat_lev;
    //     TRACE_DEBUG(1, TRACE_CORE_0,
    //                 "bat info %d,%" PRIu8 ",%" PRIu16 "\n",
    //                 is_charging, bat_lev, voltage);
    // }
    // TRACE_DEBUG(0, TRACE_MAIN, "GET BATTERY DONE\n");

    // mpu9250::init();
    // float data[3];



    // TRACE_DEBUG(0, TRACE_MAIN, "WAITING FOR TIME\n");
    // while (sensors_data.current_time.is_valid() == false) {
    //     cycle_get_gps_data();
    // }
    // TRACE_DEBUG(0, TRACE_MAIN, "TIME RECEIVED\n");
}

int Data_Actor::loop(void)
{
    absolute_time_t frameStart = get_absolute_time();

    // data update
    loop_frame_update();

    absolute_time_t frameEnd = get_absolute_time();
    auto frameTimeUs = absolute_time_diff_us(frameStart, frameEnd);
    int64_t timeToSleep = fpsToUs(DATA_PER_SECOND) - frameTimeUs;
    TRACE_DEBUG(2, TRACE_CORE_0, "frame took %" PRIi64 " should be %" PRIi64 " delta %" PRIi64 "\n",
                   frameTimeUs, fpsToUs(DATA_PER_SECOND), timeToSleep);
    sleep_us(std::max(timeToSleep, (int64_t)0));
    return 1;
}

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


int Data_Actor::loop_frame_update()
{
    Data_Actor::get_instance().handle_all();
    {
        datetime_t t;
        rtc_get_datetime(&t);
        sensors_data.current_time.from_date_time(t);
    }
    cycle_print_heart_beat();

    if(sim868::is_booted())
    {
        cycle_get_gps_data();
        cycle_get_battery_status();
    }
    else
    {
        //boot sim
        if(sim868::check_for_boot() && !sim868::is_booted())
        {
            PRINT("waiting for boot");
        }
        // PRINT("Booting done");
    }
    cycle_log_data();
    cycle_get_weather_data();
    cycle_get_slope();


    sensors_data.imu.accel_hist.add_element((float)sensors_data.imu.rotation_speed.y);

    // PRINTF("%f\n", (float)sensors_data.imu.rotation_speed.y);




    // read data
    const float distance = speed_cadence_update();
    gear_update(gear_suggestion_calc);


    // simulate slowing down
    #ifdef SIM_SLOW_DOWN
    if(session_p->is_running())
    {
        CYCLE_UPDATE_SIMPLE(true, 200,
        {
            send_speed_comp(speed::get_velocity_kph_raw(), sensors_data.velocity);
        });
        static int single_trig = 0;
        if(single_trig == 0)
        {
            PRINT("[INFO] SIM STARTED");
            single_trig++;
            speed_emulate_slowing(20, -3.0);
        }
    }
    #endif

    // update if running
    if(session_p != nullptr && session_p->has_started())
    {
        session_p->update(sensors_data.velocity, distance);
    }

    if(sensors_data.velocity == 0.0)
    {
        update_total_stats();
    }

    return 0;
}

static float calc_slope(const float distance, const float altitude)
{
    static float last_distance = 0.0f;
    static float last_altitude = 0.0f;
    static float slope;
    // check if last_distance is no bigger than current distance
    if(distance >= last_distance + 20.0)
    {

        const float current_alt = altitude;
        const float d_alt = current_alt - last_altitude;
        const float d_dis = distance - last_distance;
        const float sin = d_alt / d_dis;
        slope = sin * 100.0f;

        last_distance = distance;
        last_altitude = current_alt;
    }
    else {
        last_distance = distance;
        slope = 0.0f;
    }
    return slope;
}

static bool load_config_from_file(const char* config_str, const char* file_name)
{
    bool success = config.from_string(config_str);
    auto name = split_string(file_name, '.');
    if(name.size() > 0)
        config.name = name.at(0);
    else
        config.name = "unknown";
    speed::set_wheel(config.wheel_size);

    return success;
}

static void update_total_stats()
{
    static float ridden_dist = 0.0f;
    ridden_dist += speed::get_distance_total();
    if(ridden_dist >= 0.1f)
    {
        // calc drive time
        const float ridden_time = speed::get_time_total();

        auto payload = new Display_Actor::Sig_Display_Actor_Total_Update();
        payload->ridden_dist = ridden_dist;
        payload->ridden_time = ridden_time;
        Signal sig(actors_common::SIG_DISPLAY_ACTOR_TOTAL_UPDATE, payload);
        Display_Actor::get_instance().send_signal(sig);

        ridden_dist = 0.0;
    }
}

static float speed_cadence_update()
{
    sensors_data.velocity = speed::get_velocity_kph();
    sensors_data.cadence = cadence::get_cadence();
    return speed::get_distance_m();
}

static void gear_update(Gear_Suggestion_Calculator* gear_suggestion_calc)
{
    // get gear
    const float wheel_rpm = speed::kph_to_rpm(sensors_data.velocity);
    const float read_ratio = wheel_rpm / sensors_data.cadence;
    sensors_data.gear = config.get_current_gear(read_ratio);

    // calc gear suggestion
    auto gear_suggestion = gear_suggestion_calc->get_suggested_gear(sensors_data.cadence, sensors_data.gear);


    switch (gear_suggestion.suggestion)
    {
    case Gear_Suggestion::UP_SHIFT:
        strncpy(sensors_data.gear_suggestions.gear_suggestion, "/\\", GEAR_SUGGESTION_LEN + 1);
        sensors_data.gear_suggestions.gear_suggestion_color = {0x0,0xf,0x0};
        break;
    case Gear_Suggestion::DOWN_SHIFT:
        strncpy(sensors_data.gear_suggestions.gear_suggestion, "\\/", GEAR_SUGGESTION_LEN + 1);
        sensors_data.gear_suggestions.gear_suggestion_color = {0xf,0x0,0x1};
        break;
    case Gear_Suggestion::NO_SHIFT:
        strncpy(sensors_data.gear_suggestions.gear_suggestion, "--", GEAR_SUGGESTION_LEN + 1);
        sensors_data.gear_suggestions.gear_suggestion_color = {0xf,0xf,0xf};
        break;
    default:
        strncpy(sensors_data.gear_suggestions.gear_suggestion, "  ", GEAR_SUGGESTION_LEN + 1);
        sensors_data.gear_suggestions.gear_suggestion_color = {0xf,0xf,0xf};
        break;
    }
}


//========================================================================
//                           CYCLE FUNCTIONS
//========================================================================


static void cycle_print_heart_beat()
{
    CYCLE_UPDATE_SIMPLE(true, HEART_BEAT_CYCLE_MS,
        {
            // float memory = (float)check_free_mem()/1000.0;
            // PRINTF("[HEART_BEAT] time since boot: %.3fs Avaible memory = %.3fkb/256kb\n", (float)to_ms_since_boot(get_absolute_time())/ 1000.0, memory);
            PRINTF("[HEART_BEAT] time since boot: %.3fs\n", (float)to_ms_since_boot(get_absolute_time())/ 1000.0);

        });
}

static void cycle_get_battery_status()
{
    bool is_charging = 0;
    uint8_t bat_lev = 0;
    uint16_t voltage = 0;
    CYCLE_UPDATE_SIMPLE_SLOW_RERUN(sim868::get_bat_level(is_charging, bat_lev, voltage), BAT_LEV_CYCLE_MS, 3000,
                 {

                     sensors_data.lipo.is_charging = is_charging;
                     sensors_data.lipo.level = bat_lev;
                                          TRACE_DEBUG(1, TRACE_CORE_0,
                                 "bat info %d,%" PRIu8 ",%" PRIu16 "\n",
                                 is_charging, bat_lev, voltage);
                 });
}

#ifndef FOLDER_LOG_DATA
#define FOLDER_LOG_DATA "log/"
#endif

static void send_log_signal(const Time_HourS& time, const  GpsDataS& gps, const Session_Data& session, const Sensor_Data& sensor_data)
{
    auto payload = new Display_Actor::Sig_Display_Actor_Log();
    std::stringstream ss;
    ss << FOLDER_LOG_DATA "gps_log_" << time_to_str_file_name_conv(session.get_start_time()) << ".csv";
    payload->file_name = ss.str();
    payload->header = "time;latitude;longitude;velocity_gps;velocity_gpio;altitude_gps;altitude_press;slope;cadence;gear\n";

    char buffer[200] = {0};
    sprintf(buffer, "%s;%f;%f;%f;%f;%f;%f;%f;%f;%d\n",
        time_to_str(time).c_str(), gps.lat, gps.lon, gps.speed, sensor_data.velocity, gps.msl, sensor_data.altitude, sensor_data.slope, sensor_data.cadence, (int)sensor_data.gear.rear);
    payload->line = buffer;

    Signal sig(actors_common::SIG_DISPLAY_ACTOR_LOG, payload);
    Display_Actor::get_instance().send_signal(sig);
}

static void cycle_log_data()
{
    CYCLE_UPDATE_SIMPLE(true, LOG_DATA_CYCLE_MS,
        {

            if(session_p->is_running() &&
                session_p->get_start_time().is_valid())
            {
                // send log to core1
                send_log_signal(sensors_data.current_time.hours, sensors_data.gps_data, *session_p, sensors_data);
            }
        });
}

static void cycle_get_gps_data()
{
    static float speed;
    static float latitude;
    static float longitude;
    static float msl;
    static TimeS current_time;
    datetime_t t;
    static int last_signal_strength = -1;
    static int last_signal_strength2 = -1;


    CYCLE_UPDATE_SIMPLE(sim868::gps::fetch_data(), GPS_FETCH_CYCLE_MS,
        {
            sim868::gps::get_speed(speed);
            sim868::gps::get_position(latitude, longitude);
            sim868::gps::get_msl(msl);

            sim868::gps::get_signal(sensors_data.gps_data.sat,
                                    sensors_data.gps_data.sat2);
            sensors_data.gps_data.speed = speed;
            sensors_data.gps_data.lat = latitude;
            sensors_data.gps_data.lon = longitude;
            sensors_data.gps_data.msl = msl;
            sim868::gps::get_date(current_time);
            if(!sensors_data.current_time.is_valid() && current_time.is_valid())
            {
                t = current_time.to_date_time();
                change_time_by_hour(&t, config.hour_offset);
                rtc_set_datetime(&t);
                sensors_data.current_time = current_time;
            }
            if(last_signal_strength != sensors_data.gps_data.sat ||
               last_signal_strength2 != sensors_data.gps_data.sat2){
                last_signal_strength = sensors_data.gps_data.sat;
                last_signal_strength2 = sensors_data.gps_data.sat2;

                TRACE_DEBUG(6, TRACE_CORE_0,
                            "date = %s signal = [%" PRIu8 ",%" PRIu8 "]\n",
                            time_to_str(current_time).c_str(),
                            sensors_data.gps_data.sat,
                            sensors_data.gps_data.sat2);
            }
            TRACE_DEBUG(3, TRACE_CORE_0,
                        "gps speed %.1f, pos [%.5f,%.5f] date = %s signal = [%" PRIu8 ",%" PRIu8 "]\n",
                        speed, latitude, longitude, time_to_str(current_time).c_str(),
                        sensors_data.gps_data.sat,
                        sensors_data.gps_data.sat2);
            });
}

static void cycle_get_weather_data()
{
    float temp, press;
    float altitude;

    CYCLE_UPDATE_SIMPLE(true, WEATHER_CYCLE_MS,
                 {
                    std::tie(temp, press) = bmp280::get_temp_press();
                    // set as base third read from sensor
                    static float start_press = 0.0;
                    //  static uint8_t req_id = 0;
                    if (start_press == 0.0)
                    {
                        start_press = press;
                    }
                    altitude = bmp280::get_height(start_press,
                                                (float)press,
                                                temp);
                     sensors_data.weather.temperature = temp;
                     sensors_data.weather.pressure = press;
                     sensors_data.altitude = altitude;
                     TRACE_DEBUG(4, TRACE_CORE_0,
                                 "Current weather temp:%.2fC press:%.2fPa altitude:%.2fm\n",
                                 temp, press, altitude);
                 });
}

static void cycle_get_slope()
{
    CYCLE_UPDATE_SIMPLE(true, 500,
        {
            sensors_data.slope = calc_slope(speed::get_distance_m(), sensors_data.altitude);
            // if (speed::get_distance_m() > 10)
            // {
            // }
        });
}
