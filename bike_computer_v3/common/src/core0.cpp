// pico includes
#include "pico/stdlib.h"

// c/c++ includes
#include <tuple>
#include <inttypes.h>
#include <string.h>
#include <iostream>

// my includes
#include "core_utils.hpp"
#include "core0.h"
#include "traces.h"
#include "common_types.h"
#include "speedometer/speedometer.hpp"
#include "interrupts/interrupts.hpp"
//#include "battery/battery.h"
#include "sim868/interface.hpp"
#include "sim868/gps.hpp"
#include "sim868/gsm.hpp"
#include "parser.hpp"


#define DATA_PER_SECOND 10
#define BAT_LEV_CYCLE_MS (15*1000)

#define GPS_FETCH_CYCLE_MS (10*1000)

// http requests per 1h
#define GSM_FETCH_CYCLE_MS (3600*1000)



// static functions
static void setup(void);
static int loop(void);

void core0_launch_thread(void)
{
    setup();
    while (loop())
    {
        tight_loop_contents();
    }
}

static void setup(void)
{
    TRACE_DEBUG(0, TRACE_MAIN, "interrupt setup core 0\n");
    interruptSetupCore0();

    // for testing purpose
    //speed_emulate();

    // sensorData.forecast.windgusts_10m.array[0] = 1.8;
    // sensorData.forecast.windgusts_10m.array[1] = 5.9;
    // sensorData.forecast.windgusts_10m.array[2] = 10.0;
    // sensorData.forecast.windgusts_10m.array[3] = 15.0;
    // sensorData.forecast.windgusts_10m.array[4] = 20.0;
    // sensorData.forecast.windgusts_10m.array[5] = 10.0;


    //turn of power led
    gpio_init(25); //power led
    gpio_set_dir(25, GPIO_OUT);
    gpio_put(25, 1);
    sim868::boot();
    mutex_enter_blocking(&sensorDataMutex);
    sensorData.current_state = SystemState::RUNNING;
    mutex_exit(&sensorDataMutex);
}

// enum class ReqId{
//     BAT,
//     NO_REQ
// };

// static uint16_t queue[ReqId::NO];


#define CYCLE_UPDATE(function, cycle, pre_code, code) \
    do{ \
        static absolute_time_t __last_update; \
        static bool __executed = false; \
        auto __current_time = get_absolute_time(); \
        if( us_to_ms(absolute_time_diff_us(__last_update, __current_time)) >  cycle || \
            (to_ms_since_boot(__current_time) < cycle && !__executed) ) \
        { \
            { \
                pre_code \
            } \
            if(function) \
            { \
                __executed = true; \
                /* success */ \
                __last_update = __current_time; \
                { \
                    code \
                } \
            } \
        } \
    } while(0)

static int loop(void)
{
    absolute_time_t frameStart = get_absolute_time();

    // data update
    { 
        // battery
        static bool is_charging = 0;
        static uint8_t bat_lev = 0;
        static uint16_t voltage = 0;
        CYCLE_UPDATE(sim868::get_bat_level(is_charging, bat_lev, voltage), BAT_LEV_CYCLE_MS,
        {
            
        },
        {
            mutex_enter_blocking(&sensorDataMutex);
            sensorData.lipo.is_charging = is_charging;
            sensorData.lipo.level = bat_lev;
            mutex_exit(&sensorDataMutex);
            TRACE_DEBUG(1,TRACE_CORE_0,
                        "bat info %d,%" PRIu8 ",%" PRIu16 "\n",
                        is_charging, bat_lev, voltage);
        });

        static float speed;
        static float latitude;
        static float longitude;
        static float msl;
        static TimeS current_time;
        CYCLE_UPDATE(sim868::gps::fetch_data(), GPS_FETCH_CYCLE_MS,
        {
            
        },
        {
            sim868::gps::get_speed(speed);
            sim868::gps::get_position(latitude, longitude);
            sim868::gps::get_date(current_time);
            sim868::gps::get_msl(msl);
            // speed = 69.3;
            // msl = 123.34;

            time_print(current_time);
            TRACE_DEBUG(3,TRACE_CORE_0,
                        "gps speed %.1f, pos [%.5f,%.5f] date year = %" PRIu16 "\n",
                        speed, latitude, longitude, current_time.year);
            // if(latitude == 0.0 || longitude == 0.0)
            // {
            //     latitude = 51.104877842621484;
            //     longitude = 17.031670433667298;
            // }
            mutex_enter_blocking(&sensorDataMutex);
            sensorData.gps_data.speed = speed;
            sensorData.gps_data.lat = latitude;
            sensorData.gps_data.lon = longitude;
            sensorData.gps_data.msl = msl;

            memcpy(&sensorData.hour, &current_time.hour, sizeof(sensorData.hour));
            memcpy(&sensorData.date, &current_time.year, sizeof(sensorData.date));
            mutex_exit(&sensorDataMutex);
        });

    
        // http req
        static std::string forecast_json;
        static std::string http_req_addr;
        static bool success;
        Time_DateS current_time_date{current_time.year, current_time.month, current_time.day}; // fix
        CYCLE_UPDATE(sim868::gsm::get_http_req(success,http_req_addr.c_str(),forecast_json), 
            GSM_FETCH_CYCLE_MS,
            {
                if(latitude < 10.0 || longitude < 10.0)
                {
                    latitude = 51.104877842621484;
                    longitude = 17.031670433667298;
                }
                //Time_DateS current_time_date{2022, 8, 24}; // fix
                //Time_DateS current_time_date{current_time.year, current_time.month, current_time.day}; // fix
                http_req_addr = sim868::gsm::construct_http_request_url(latitude,
                                                                        longitude,
                                                                        current_time_date,
                                                                        current_time_date);
            },
            {
                {
                    if(success)
                    {
                        std::cout << "forecast: " << forecast_json << std::endl;
                    }
                    else
                    {
                        std::cout << "forecast failed" << std::endl;
                        __executed = false; // ?? 
                    }
                    auto forecast_raw = parse_json(forecast_json, 1);
                    if(forecast_raw != nullptr)
                    {
                        mutex_enter_blocking(&sensorDataMutex);
                        move_forecasts_to_forecastplots(forecast_raw, &sensorData.forecast, current_time.hour);
                        mutex_exit(&sensorDataMutex);
                    }
                    else
                    {
                        TRACE_ABNORMAL(TRACE_CORE_0, "parser returned null ptr %d\n", 0);
                    }
                }
            });
        
        mutex_enter_blocking(&sensorDataMutex);
        // std::cout << "Core0: ";
        // print(sensorData.forecast.windgusts_10m);
        // std::cout << "len: " << sensorData.forecast.len << std::endl;

        if(sensorData.current_state != SystemState::PAUSED)
        {
            speedDataUpdate(sensorData.speed);
            sensorData.time.t = to_ms_since_boot(get_absolute_time()) / 1000;
        }


        mutex_exit(&sensorDataMutex);
    }
        
    absolute_time_t frameEnd = get_absolute_time();
    auto frameTimeUs = absolute_time_diff_us(frameStart, frameEnd);
    if(fpsToUs(DATA_PER_SECOND) > frameTimeUs)
    {
        // frame took less time
        int64_t timeToSleep = fpsToUs(DATA_PER_SECOND) - frameTimeUs; 
        sleep_us(timeToSleep);
        TRACE_DEBUG(2, TRACE_CORE_0,
                    "frame took %" PRIi64 " max time is %" PRIi64 " sleeping %" PRIi64 "\n",
                    frameTimeUs, fpsToUs(DATA_PER_SECOND), timeToSleep);
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



