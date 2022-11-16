// #-------------------------------#
// |           includes            |
// #-------------------------------#
// pico includes
#include <pico/stdlib.h>
#include "display/driver.hpp"
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
#include "core0.h"
#include "signals.hpp"
#include "traces.h"
#include "common_types.h"
#include "common_data.hpp"
#include "speedometer/speedometer.hpp"
#include "cadence/cadence.hpp"
#include "interrupts/interrupts.hpp"
#include "parser.hpp"
#include "common_actors.hpp"
#include "sd_file.h"
// SIM868
#include "sim868/interface.hpp"
#include "sim868/gps.hpp"
#include "sim868/gsm.hpp"
// BMP
#include "IMU.h"
#include "I2C.h"

#include "hardware/rtc.h"

// #-------------------------------#
// |            macros             |
// #-------------------------------#
#define DATA_PER_SECOND 1

// cycles times
#define BAT_LEV_CYCLE_MS (29*1000)
#define WEATHER_CYCLE_MS (1*1000)
#define HEART_BEAT_CYCLE_MS (10*1000)
#define GPS_FETCH_CYCLE_MS (1*1000)
#define TIME_FETCH_CYCLE_MS (10*1000)
// http requests per 10min
#define GSM_FETCH_CYCLE_MS (10*60*1000)

#define LOG_DATA_CYCLE_MS (1 * 1000)



//switches
#define SIM_WHEEL_CADENCE 1
#define PREDEFINED_BIKE_SETUP 0


// #------------------------------#
// | global variables definitions |
// #------------------------------#


// #------------------------------#
// | static variables definitions |
// #------------------------------#
static Bike_Config_S config;
static bool config_received = false;
int local_time_offset = 0;

// #------------------------------#
// | static functions declarations|
// #------------------------------#
static void setup(void);
static int loop(void);
static int loop_frame_update();

static bool load_config_from_file(const char* config_str, const char* file_name);

static void update_total_stats();

static void cycle_log_data();
static void cycle_print_heart_beat();
static void cycle_get_battery_status();
static void cycle_get_gps_data();
static void cycle_get_forecast_data();
static void cycle_get_weather_data();

template<size_t N>
static inline void move_forecasts_to_forecastplots(ForecastS* raw_data, ForecastArrS<N>* forecast, uint8_t current_hour=0);

// #------------------------------#
// | global function definitions  |
// #------------------------------#
void core0_launch_thread(void)
{
    setup();
    while (loop())
    {
        tight_loop_contents();
    }
}

// #------------------------------#
// | static functions definitions |
// #------------------------------#
static void setup(void)
{
    TRACE_DEBUG(0, TRACE_MAIN, "interrupt setup core 0\n");
    interruptSetupCore0();

    rtc_init();
    datetime_t t = {
        .year  = 2000,
        .month = 01,
        .day   = 01,
        .dotw  = 0, // 0 is Sunday, so 5 is Friday
        .hour  = 0,
        .min   = 0,
        .sec   = 00
    };
    rtc_set_datetime(&t);

#if PREDEFINED_BIKE_SETUP == 1
    // uint8_t gears_front[] = {32};
    // uint8_t gears_rear[] = {51, 45, 39, 33, 28, 24, 21, 18, 15, 13, 11};
    // BIKE_CONFIG_SET_GEARS(config, gears_front, gears_rear);
    config.from_string(
        "GF:32\n"
        "GR:51,45,39,33,28,24,21,18,15,13,11\n"
        "WS:2.186484\n"
    );
    // config.to_string();
    //PRINTF("%s\n", config.to_string());

    // Sd_File config_file("giant_trance.cfg");
    // config_file.clear();
    // config_file.append(
    //     "GF:32\n"
    //     "GR:51,45,39,33,28,24,21,18,15,13,11\n"
    //     "WS:2.186484\n"
    // );
    // load_config_from_file("giant_trance.cfg");
#else

    // wait for config
    while(!config_received)
    {
        actor_core0.handle_all();
        sleep_ms(10);
    }
    config.to_string();
#endif

    // for testing purpose
    if(SIM_WHEEL_CADENCE) // TODO true when usb connected
    {
        float emulated_speed = 20.0;
        speed_emulate(emulated_speed);
        PRINTF("emulated_speed=%f\n", emulated_speed);
        float wheel_rpm = speed::kph_to_rpm(emulated_speed);
        PRINTF("wheel_rpm=%f\n",wheel_rpm);
        float ratio = config.get_gear_ratio({1,10});
        PRINTF("ratio=%f\n",ratio);
        float cadence = wheel_rpm / ratio;
        PRINTF("cadence=%f\n",cadence);
        cadence::emulate(cadence);
    }

    // sensors_data.forecast.windgusts_10m.array[0] = 1.8;
    // sensors_data.forecast.windgusts_10m.array[1] = 5.9;
    // sensors_data.forecast.windgusts_10m.array[2] = 10.0;
    // sensors_data.forecast.windgusts_10m.array[3] = 15.0;
    // sensors_data.forecast.windgusts_10m.array[4] = 20.0;
    // sensors_data.forecast.windgusts_10m.array[5] = 10.0;

    //turn of power led
    gpio_init(25); //power led
    gpio_set_dir(25, GPIO_OUT);
    gpio_put(25, 1);

    // setup sim868
   sim868::init();
   sim868::turnOn();

    mutex_enter_blocking(&sensorDataMutex);
    session_p = new Session_Data();

    sensors_data.current_state = SystemState::TURNED_ON;
    mutex_exit(&sensorDataMutex);

    //I2C_Init();
    IMU_Init();
}

static int loop(void)
{
    absolute_time_t frameStart = get_absolute_time();

    // data update
    loop_frame_update();

    absolute_time_t frameEnd = get_absolute_time();
    auto frameTimeUs = absolute_time_diff_us(frameStart, frameEnd);

    if(fpsToUs(DATA_PER_SECOND) > frameTimeUs)
    {
        // frame took less time
        int64_t timeToSleep = fpsToUs(DATA_PER_SECOND) - frameTimeUs;
        TRACE_DEBUG(2, TRACE_CORE_0,
                    "frame took %" PRIi64 " max time is %" PRIi64 " sleeping %" PRIi64 "\n",
                    frameTimeUs, fpsToUs(DATA_PER_SECOND), timeToSleep);
        // TODO handle here signals instead of sleep
        sleep_us(timeToSleep);

    }
    else
    {
        int64_t timeToSleep = fpsToUs(DATA_PER_SECOND) - frameTimeUs;
        TRACE_ABNORMAL(TRACE_CORE_0,
                       "frame took %" PRIi64 " should be %" PRIi64 " delta %" PRIi64 "\n",
                       frameTimeUs, fpsToUs(DATA_PER_SECOND), timeToSleep);
    }
    return 1;
}

void Core0::handle_sig_set_config(const Signal &sig)
{
    const auto payload = sig.get_payload<Sig_Core0_Set_Config*>();
    config_received = load_config_from_file(payload->file_content.c_str(), payload->file_name.c_str());
    if(!config_received)
    {
        // send msg to user

        auto payload = new Sig_Core1_Show_Msg();
        *payload = {
            std::string("Cannot load bike config"),
            {0xf,0xf,0xf},
            0
        };
        Signal sig(SIG_CORE1_SHOW_MSG, payload);
        actor_core1.send_signal(sig);
    }
    else
    {
        config.to_string();
    }
    delete payload;
}

void Core0::handle_sig_set_total(const Signal &sig)
{
    const auto payload = sig.get_payload<Sig_Core0_Set_Total*>();
    Unique_Mutex mutex_lock(&sensorDataMutex);
    sensors_data.total_time_ridden = payload->ridden_time_total;
    sensors_data.total_distance_ridden = payload->ridden_dist_total;
    delete payload;
}


void Core0::handle_sig_pause(const Signal &sig)
{
    speed::stop();
    Unique_Mutex mutex_lock(&sensorDataMutex);
    sensors_data.current_state = SystemState::PAUSED;
    session_p->pause();
}
void Core0::handle_sig_continue(const Signal &sig)
{
    speed::start();
    Unique_Mutex mutex_lock(&sensorDataMutex);
    sensors_data.current_state = SystemState::RUNNING;
    session_p->cont();
}

void Core0::handle_sig_get_file_resond(const Signal &sig)
{
    auto payload = sig.get_payload<Sig_Core0_Get_File_Respond*>();
    switch (payload->type) {
        case File_Respond::time_offset:
        {
            auto offset = std::atoi(payload->file_content.c_str());
                datetime_t t;
                rtc_get_datetime(&t);
                change_time_by_hour(&t, offset);
                rtc_set_datetime(&t);

        }
        break;
        default:
        {
            TRACE_ABNORMAL(TRACE_CORE_0, "unhandled file respond type %d\n", payload->type);
        }
    }

    delete payload;

}


void Core0::handle_sig_session_start(const Signal &sig)
{
    speed::stop();
    speed::reset();
    {
        Unique_Mutex mutex_lock(&sensorDataMutex);
        sensors_data.current_state = SystemState::AUTOSTART;
        if(session_p != nullptr)
            delete session_p;
        session_p = new Session_Data();
        session_p->start(sensors_data.current_time);
        session_p->pause();
    }
    speed::start();
}

void Core0::handle_sig_start(const Signal &sig)
{
    Unique_Mutex mutex_lock(&sensorDataMutex);
    session_p->pause();
    sensors_data.current_state = SystemState::AUTOSTART;
    speed::start();
    // session_p->start(sensors_data.current_time);

}
void Core0::handle_sig_stop(const Signal &sig)
{
    speed::stop();
    Unique_Mutex mutex_lock(&sensorDataMutex);
    sensors_data.current_state = SystemState::TURNED_ON;
    session_p->end(sensors_data.current_time);
}


static void on_stop();
static void on_stop()
{
    update_total_stats();
}


static int loop_frame_update()
{
    // update_total_stats();

    actor_core0.handle_all();
    {
        Unique_Mutex mutex(&sensorDataMutex);
        datetime_t t;
        rtc_get_datetime(&t);
        sensors_data.current_time.from_date_time(t);
    }
    cycle_print_heart_beat();

    if(sim868::is_booted())
    { // sim usage
        cycle_get_gps_data();

        // battery
        cycle_get_battery_status();
        cycle_get_weather_data();
        cycle_log_data();
    }
    else
    {
        PRINT("Booting sim868");

        //boot sim
        if(sim868::check_for_boot() && !sim868::is_booted())
        {
            // not booted
            PRINT("waiting for boot");
        }
        else
        {
            PRINT("Booting done");
        }
    }




    {
        Unique_Mutex mutex(&sensorDataMutex);
        if(session_p != nullptr)
        {
            auto time_start = session_p->get_start_time();
            if(!time_start.is_valid() && sensors_data.current_time.is_valid())
            {
                const auto current_absolute_time = get_absolute_time();
                const auto start_absolute_time = session_p->get_start_absolute_time();
                const auto diff_ms = us_to_ms(absolute_time_diff_us(start_absolute_time, current_absolute_time));

                // calc how many hours to remove
                time_start = sensors_data.current_time;

                time_start.substract_ms(diff_ms);
                session_p->set_start_time(time_start);
            }
        }
    }



    // calc delta
    const absolute_time_t time_update_current = get_absolute_time();
    static absolute_time_t time_update_prev;
    const auto delta_ms = us_to_ms(absolute_time_diff_us(time_update_prev, time_update_current));
    time_update_prev = time_update_current;

    // read data first
    static float last_velocity;
    const float velocity = speed::get_velocity_kph();
    const float cadence = cadence::get_cadence();

    const float accel = (velocity - last_velocity)/((double)delta_ms / 1000.0);
    last_velocity = velocity;

    // get gear
    const float wheel_rpm = speed::kph_to_rpm(velocity);
    const float read_ratio = wheel_rpm / cadence;
    const Gear_S gear = config.get_current_gear(read_ratio);

    // this gets distance without paused fragments
    const float distance = speed::get_distance_m();

    {
        static float last_distance = 0.0f;
        static float last_altitude = 0.0f;
        if(distance - last_distance > 5.0f)
        {
            Unique_Mutex mutex(&sensorDataMutex);
            const float current_alt = sensors_data.altitude;
            const float d_alt = current_alt - last_altitude;
            const float d_dis = distance - last_distance;
            const float tg = d_alt / d_dis;
            sensors_data.slope = tg * 100.0f;

            last_distance = distance;
            last_altitude = current_alt;
        }
    }
    // calc gear suggestion
    Gear_Suggestion_Calculator suggestion(config);
    auto gear_suggestion = suggestion.get_suggested_gear(cadence, accel, gear);

    TRACE_DEBUG(5, TRACE_CORE_0,
        "dist=%f wheel_rpm=%f, cadence=%f, read_ratio=%f, gear={%" PRIu8 ",%" PRIu8 "}\n",
        distance, wheel_rpm, cadence, read_ratio, gear.front, gear.rear);


    {
        static uint8_t last_gear = 0;
        if(gear.rear != last_gear)
        {
            last_gear = gear.rear;
            Session_Data session;
            {
                Unique_Mutex mutex(&sensorDataMutex);
                session = *session_p;
            }
            if(session.is_running() && session.get_start_time().is_valid())
            {
                auto payload = new Sig_Core1_Log();
                {
                    std::stringstream ss;
                    ss << "gear_log_" << time_to_str_file_name_conv(session.get_start_time()) << ".csv";
                    payload->file_name = ss.str();
                }
                payload->header = "accel;wheel_rpm;cadence;read_ratio;gear\n";
                {
                    std::stringstream ss;
                    ss << accel << ";" << wheel_rpm << ";" << cadence << ";" << read_ratio << ";" << (int)gear.rear << "\n";
                    payload->line = ss.str();
                }

                Signal sig(SIG_CORE1_LOG, payload);
                actor_core1.send_signal(sig);
            }
        }
    }



    // update sensor
    mutex_enter_blocking(&sensorDataMutex);
    sensors_data.velocity = velocity;
    sensors_data.cadence = cadence;
    sensors_data.accel = accel;
    sensors_data.gear = gear;

    sensors_data.gear_suggestions.cadence_min = gear_suggestion.cadence_min;
    sensors_data.gear_suggestions.cadence_max = gear_suggestion.cadence_max;

    switch (gear_suggestion.suggestion)
    {
    case Gear_Suggestion::UP_SHIFT:
        strncpy(sensors_data.gear_suggestions.gear_suggestion, "/\\", GEAR_SUGGESTION_LEN);
        sensors_data.gear_suggestions.gear_suggestion_color = {0x0,0xf,0x0};
        break;
    case Gear_Suggestion::DOWN_SHIFT:
        strncpy(sensors_data.gear_suggestions.gear_suggestion, "\\/", GEAR_SUGGESTION_LEN);
        sensors_data.gear_suggestions.gear_suggestion_color = {0xf,0x0,0x1};
        break;
    case Gear_Suggestion::NO_SHIFT:
        strncpy(sensors_data.gear_suggestions.gear_suggestion, "--", GEAR_SUGGESTION_LEN);
        sensors_data.gear_suggestions.gear_suggestion_color = {0xf,0xf,0xf};
        break;
    default:
        strncpy(sensors_data.gear_suggestions.gear_suggestion, "  ", GEAR_SUGGESTION_LEN);
        sensors_data.gear_suggestions.gear_suggestion_color = {0xf,0xf,0xf};
        break;
    }
    // sensors_data.cadence_min = cadence_min;
    // sensors_data.cadence_max = cadence_max;

    // strncpy(sensors_data.gear_suggestion, gear_suggestion, GEAR_SUGGESTION_LEN);
    // sensors_data.gear_suggestion[GEAR_SUGGESTION_LEN] = '\0';
    // sensors_data.gear_suggestion_color = gear_suggestion_color;

    // update if running
    session_p->update(velocity, distance);
    {
        static uint8_t last_gear_idx = 0;
        uint8_t current_gear_idx = config.to_idx(gear);
        if (last_gear_idx == current_gear_idx)
        {
            // add gear time if is running
            session_p->add_gear_time(current_gear_idx, cadence, delta_ms);
        }
        last_gear_idx = current_gear_idx;
    }
    const auto state = sensors_data.current_state;
    mutex_exit(&sensorDataMutex);

    switch (state)
    {
        case SystemState::AUTOSTART:
            if(velocity > 0.0)
            {
                // TODO send start signal
                PRINTF("AUTOSTART\n");
                actor_core0.send_signal(SIG_CORE0_CONTINUE);
            }

        case SystemState::ENDED:
        case SystemState::PAUSED:
        case SystemState::RUNNING:
        if(velocity == 0.0)
        {
            actor_core0.send_signal(SIG_CORE0_START);
            on_stop();
        }
        case SystemState::TURNED_ON:
        case SystemState::CHARGING:
        default:
            break;
    }
    //memcpy(&sensors_data.hour, &current_time.hour, sizeof(sensors_data.hour));
    // std::cout << "Core0: \n";
    //  print(sensors_data.forecast.windgusts_10m);
    //  std::cout << "len: " << sensors_data.forecast.len << std::endl;
    // speedDataUpdate(sensors_data.speed, sensors_data.current_state);
    //sensors_data.time.t = to_ms_since_boot(get_absolute_time()) / 1000;

    actor_core0.handle_all();
    return 0;
}

static bool load_config_from_file(const char* config_str, const char* file_name)
{
    //Sd_File config_file(file_name);
    //const std::string config_str = config_file.read_all();
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

        auto payload = new Sig_Core1_Total_Update();
        payload->ridden_dist = ridden_dist;
        payload->ridden_time = ridden_time;
        Signal sig(SIG_CORE1_TOTAL_UPDATE, payload);
        actor_core1.send_signal(sig);

        ridden_dist = 0.0;
    }
}


//========================================================================
//                           CYCLE FUNCTIONS
//========================================================================


static void cycle_print_heart_beat()
{
    size_t avaible_memory;
    CYCLE_UPDATE_SIMPLE(true, HEART_BEAT_CYCLE_MS,
        {
            float memory = (float)check_free_mem()/1000.0;
            PRINTF("[HEART_BEAT] time since boot: %.3fs Avaible memory = %.3fkb/256kb\n", (float)to_ms_since_boot(get_absolute_time())/ 1000.0, memory);
        });
}

static void cycle_get_battery_status()
{
    bool is_charging = 0;
    uint8_t bat_lev = 0;
    uint16_t voltage = 0;
    CYCLE_UPDATE_SIMPLE_SLOW_RERUN(sim868::get_bat_level(is_charging, bat_lev, voltage), BAT_LEV_CYCLE_MS, 500,
                 {
                     mutex_enter_blocking(&sensorDataMutex);
                     sensors_data.lipo.is_charging = is_charging;
                     sensors_data.lipo.level = bat_lev;
                     mutex_exit(&sensorDataMutex);
                     TRACE_DEBUG(1, TRACE_CORE_0,
                                 "bat info %d,%" PRIu8 ",%" PRIu16 "\n",
                                 is_charging, bat_lev, voltage);
                 });
}

static void send_log_signal(const Time_HourS& time, const  GpsDataS& gps, const Session_Data& session, const Sensor_Data& sensor_data)
{
    auto payload = new Sig_Core1_Log();
    std::stringstream ss;
    ss << "gps_log_" << time_to_str_file_name_conv(session.get_start_time()) << ".csv";
    payload->file_name = ss.str();
    payload->header = "time;latitude;longitude;velocity_gps;velocity_gpio;altitude_gps;altitude_press\n";

    char buffer[64] = {0};
    sprintf(buffer, "%s;%f;%f;%f;%f;%f;%f\n", time_to_str(time).c_str(), gps.lat, gps.lon, gps.speed, sensor_data.velocity, gps.msl, sensor_data.altitude);
    payload->line = buffer;

    Signal sig(SIG_CORE1_LOG, payload);
    actor_core1.send_signal(sig);
}

static void cycle_log_data()
{
    CYCLE_UPDATE_SIMPLE(true, LOG_DATA_CYCLE_MS,
        {
            mutex_enter_blocking(&sensorDataMutex);
            if(sensors_data.gps_data.lat != 0 &&
            sensors_data.gps_data.lon != 0 &&
            session_p->is_running() &&
            session_p->get_start_time().is_valid())
            {
                // send log to core1
                send_log_signal(sensors_data.current_time.hours, sensors_data.gps_data, *session_p, sensors_data);
            }
            mutex_exit(&sensorDataMutex);
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

    auto get_time_offset = [](void)
    {
        auto payload = new Sig_Core1_Get_File();
        payload->type = File_Respond::time_offset;
        payload->file_name = "time_offset.txt";
        Signal sig(SIG_CORE1_GET_FILE, payload);
        actor_core1.send_signal(sig);
    };
    CYCLE_UPDATE_SIMPLE(sim868::gps::fetch_data(), GPS_FETCH_CYCLE_MS,
        {
            sim868::gps::get_speed(speed);
            sim868::gps::get_position(latitude, longitude);
            sim868::gps::get_msl(msl);
            // speed = 69.3;
            // msl = 123.34;

            // time_print(current_time);

            // if(latitude == 0.0 || longitude == 0.0)
            // {
            //     latitude = 51.104877842621484;
            //     longitude = 17.031670433667298;
            // }
            mutex_enter_blocking(&sensorDataMutex);
            sim868::gps::get_signal(sensors_data.gps_data.sat,
                                    sensors_data.gps_data.sat2);
            sensors_data.gps_data.speed = speed;
            sensors_data.gps_data.lat = latitude;
            sensors_data.gps_data.lon = longitude;
            sensors_data.gps_data.msl = msl;
            sim868::gps::get_date(current_time);
            // if(current_time.year > current_time.year)
            if(!sensors_data.current_time.is_valid() && current_time.is_valid())
            {
                t = current_time.to_date_time();
                rtc_set_datetime(&t);
                sensors_data.current_time = current_time;
                PRINT("set time");
                get_time_offset();
            }

            mutex_exit(&sensorDataMutex);
            TRACE_DEBUG(3, TRACE_CORE_0,
                        "gps speed %.1f, pos [%.5f,%.5f] date = %s signal = [%" PRIu8 ",%" PRIu8 "]\n",
                        speed, latitude, longitude, time_to_str(current_time).c_str(),
                        sensors_data.gps_data.sat,
                        sensors_data.gps_data.sat2);
        });
}

static void cycle_get_forecast_data()
{
    // http req
    static std::string forecast_json;
    static std::string http_req_addr;
    static bool success;

    mutex_enter_blocking(&sensorDataMutex);
    TimeS current_time = sensors_data.current_time;
    float latitude = sensors_data.gps_data.lat;
    float longitude = sensors_data.gps_data.lon;
    mutex_exit(&sensorDataMutex);
    if(current_time.is_valid() && (latitude > 0.0 || longitude > 0.0) )
    {

        CYCLE_UPDATE(sim868::gsm::get_http_req(success,http_req_addr.c_str(),forecast_json),
            !success,
            GSM_FETCH_CYCLE_MS,
            {
                if (current_time.is_valid() )
                {
                    http_req_addr = sim868::gsm::construct_http_request_url(latitude,
                                                                            longitude,
                                                                            current_time.date, // TODO if less than <DEFINE> to next day add daya + 1
                                                                            current_time.date);
                }
                else
                {
                    http_req_addr = sim868::gsm::construct_http_request_url_no_date(latitude, longitude);
                }
            },
            {
                {
                    if(success)
                    {
                        PRINT("forecast: '" << forecast_json << "'");
                        auto daycnt = 1;
                        if(current_time.year == 0)
                            daycnt = 7;
                        auto forecast_raw = parse_json(forecast_json, daycnt);
                        if(forecast_raw != nullptr)
                        {
                            mutex_enter_blocking(&sensorDataMutex);
                            move_forecasts_to_forecastplots(forecast_raw, &sensors_data.forecast, current_time.hour);
                            mutex_exit(&sensorDataMutex);
                        }
                        else
                        {
                            TRACE_ABNORMAL(TRACE_CORE_0, "parser returned null ptr\n");
                        }
                    }
                    else
                    {
                        TRACE_ABNORMAL(TRACE_CORE_0, "forecast failed\n");
                        //__executed = false; // ??
                    }
                }
            });
    }
}

static void cycle_get_weather_data()
{
    int32_t temp, press;
    float altitude;
    TimeS current_time{0, 0, 0, 0, 0, 0.0f}; // fix xd
    CYCLE_UPDATE_SIMPLE(true, WEATHER_CYCLE_MS,
                 {
                     std::tie(temp, press) = bmp280::get_temp_press();
                     mutex_enter_blocking(&sensorDataMutex);
                     if (sensors_data.forecast.pressure_msl.array[current_time.hour] != 0)
                     {
                         altitude = bmp280::get_height(sensors_data.forecast.pressure_msl.array[current_time.hour] * 1000.0f,
                                                       (float)press,
                                                       (float)temp / 100.0);
                     }
                     else
                     {
                        // set as base third read from sensor
                         static float start_press = 0.0;
                         static uint8_t req_id = 0;
                         if (req_id < 2) // (10*1000 / WEATHER_CYCLE_MS)
                         {
                             start_press = (float)press;
                             req_id++;
                         }
                         altitude = bmp280::get_height(start_press,
                                                       (float)press,
                                                       (float)temp / 100.0);
                     }
                     // temp = (temp % 100) < 50 ? (temp / 100) : ((temp / 100) + 1);
                     sensors_data.weather.temperature = (float)temp / 100.0f;
                     sensors_data.weather.pressure = press;
                     sensors_data.altitude = altitude;
                     mutex_exit(&sensorDataMutex);
                     TRACE_DEBUG(4, TRACE_CORE_0,
                                 "Current weather temp:%" PRId32 "C press:%" PRId32 "Pa altitude:%.2fm\n",
                                 temp, press, altitude);
                 });
}


template<size_t N>
static inline void move_forecasts_to_forecastplots(ForecastS* raw_data, ForecastArrS<N>* forecast, uint8_t current_hour)
{
    if(current_hour + N > raw_data->data_len)
    {
        current_hour = raw_data->data_len - N;
    }
    auto hour = current_hour;
    for(size_t i = 0; i < N; i++)
    {
        forecast->time_h.array[i] = hour++;
    }
    memcpy(&forecast->temperature_2m.array[0], &raw_data->temperature_2m[current_hour], sizeof(*raw_data->temperature_2m) * FORECAST_SENSOR_DATA_LEN);
    memcpy(&forecast->pressure_msl.array[0], &raw_data->pressure_msl[current_hour], sizeof(*raw_data->pressure_msl) * FORECAST_SENSOR_DATA_LEN);
    memcpy(&forecast->precipitation.array[0], &raw_data->precipitation[current_hour], sizeof(*raw_data->precipitation) * FORECAST_SENSOR_DATA_LEN);
    memcpy(&forecast->windspeed_10m.array[0], &raw_data->windspeed_10m[current_hour], sizeof(*raw_data->windspeed_10m) * FORECAST_SENSOR_DATA_LEN);
    memcpy(&forecast->winddirection_10m.array[0], &raw_data->winddirection_10m[current_hour], sizeof(*raw_data->winddirection_10m) * FORECAST_SENSOR_DATA_LEN);
    memcpy(&forecast->windgusts_10m.array[0], &raw_data->windgusts_10m[current_hour], sizeof(*raw_data->windgusts_10m) * FORECAST_SENSOR_DATA_LEN);

    forecast->sunrise.hour = raw_data->sunrise[0].hour;
    forecast->sunrise.minutes = raw_data->sunrise[0].minutes;
    forecast->sunrise.seconds = 0;

    forecast->sunset.hour = raw_data->sunset[0].hour;
    forecast->sunset.minutes = raw_data->sunset[0].minutes;
    forecast->sunset.seconds = 0;

    forecast->winddirection_10m_dominant = raw_data->winddirection_10m_dominant[0];
    forecast->current_windspeed = raw_data->current_weather.windspeed;
    forecast->current_winddirection = raw_data->current_weather.winddirection;

    delete raw_data;
}
