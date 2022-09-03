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
#include "parser.hpp"
// SIM868
#include "sim868/interface.hpp"
#include "sim868/gps.hpp"
#include "sim868/gsm.hpp"
// BMP
#include "IMU.h"
#include "I2C.h"


#define DATA_PER_SECOND 10
#define BAT_LEV_CYCLE_MS (29*1000)
#define WEATHER_CYCLE_MS (10*1000)

#define HEART_BEAT_CYCLE_MS (1*1000)


#define GPS_FETCH_CYCLE_MS (5*1000)

// http requests per 10min
#define GSM_FETCH_CYCLE_MS (10*60*1000)



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
    //memcpy(sensorData.cipgsmloc, "0,0.000000,0.000000", sizeof("0,0.000000,0.000000"));
    sensorData.current_state = SystemState::AUTOSTART;
    mutex_exit(&sensorDataMutex);

    I2C_Init();
    IMU_Init();
}

static int loop(void)
{
    absolute_time_t frameStart = get_absolute_time();

    // data update
    {
        CYCLE_UPDATE(true, false, HEART_BEAT_CYCLE_MS,
        {},{
            //PRINTF("[HEART_BEAT] time since boot: %.3fs\n", (float)to_ms_since_boot(get_absolute_time())/ 1000.0);
        });
        //size_t avaible_memory = check_free_mem();
        //printf("Avaible memory = %zu\n", avaible_memory);

        // battery
        static bool is_charging = 0;
        static uint8_t bat_lev = 0;
        static uint16_t voltage = 0;
        CYCLE_UPDATE(sim868::get_bat_level(is_charging, bat_lev, voltage), false, BAT_LEV_CYCLE_MS,
        {
            //TRACE_DEBUG(1,TRACE_CORE_0,
            //            "Entering get_bat_level\n");
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
        // CYCLE_UPDATE(sim868::gps::fetch_data(), false, GPS_FETCH_CYCLE_MS,
        // {
        //     //TRACE_DEBUG(3,TRACE_CORE_0,
        //     //            "entering fetch_data\n");
        // },
        // {
        //     sim868::gps::get_speed(speed);
        //     sim868::gps::get_position(latitude, longitude);
        //     sim868::gps::get_msl(msl);
        //     // speed = 69.3;
        //     // msl = 123.34;

        //     time_print(current_time);
            
        //     // if(latitude == 0.0 || longitude == 0.0)
        //     // {
        //     //     latitude = 51.104877842621484;
        //     //     longitude = 17.031670433667298;
        //     // }
        //     mutex_enter_blocking(&sensorDataMutex);
        //     sim868::gps::get_signal(sensorData.gps_data.sat,
        //                             sensorData.gps_data.sat2);
        //     sensorData.gps_data.speed = speed;
        //     sensorData.gps_data.lat = latitude;
        //     sensorData.gps_data.lon = longitude;
        //     sensorData.gps_data.msl = msl;

        //     memcpy(&sensorData.date, &current_time.year, sizeof(sensorData.date));
        //     mutex_exit(&sensorDataMutex);
        //     TRACE_DEBUG(3,TRACE_CORE_0,
        //                 "gps speed %.1f, pos [%.5f,%.5f] date year = %" PRIu16 " signal = [%" PRIu8 ",%" PRIu8 "]\n",
        //                 speed, latitude, longitude, current_time.year,
        //                 sensorData.gps_data.sat,
        //                 sensorData.gps_data.sat2);
        // });

    
        // http req
        static std::string forecast_json;
        static std::string http_req_addr;
        static bool success;
        Time_DateS current_time_date{current_time.year, current_time.month, current_time.day}; // fix
        if(current_time_date.year != 0)
        {

            CYCLE_UPDATE(sim868::gsm::get_http_req(success,http_req_addr.c_str(),forecast_json),
                !success,
                GSM_FETCH_CYCLE_MS,
                {
                    // if(!success && us_to_ms(absolute_time_diff_us(__last_update, __current_time)) < 1000)
                    // {
                    //     break;
                    // }
                    if(latitude < 10.0 || longitude < 10.0)
                    {
                        latitude = 51.4;
                        longitude = 16.59;
                    }
                    http_req_addr = sim868::gsm::construct_http_request_url(latitude,
                                                                            longitude,
                                                                            current_time_date,
                                                                            current_time_date);
                },
                {
                    {
                        if(success)
                        {
                            std::cout << "forecast: '" << forecast_json << "'" << std::endl;
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
                        else
                        {
                            std::cout << "forecast failed" << std::endl;
                            //__executed = false; // ?? 
                        }                   
                    }
                });
        }

        int32_t temp, press;
        float altitude;
        CYCLE_UPDATE(true, false, WEATHER_CYCLE_MS,
        {},
        {
            std::tie(temp, press) = bmp280::get_temp_press();
            mutex_enter_blocking(&sensorDataMutex);
            if(sensorData.forecast.pressure_msl.array[current_time.hour] != 0)
            {
                altitude = bmp280::get_height(sensorData.forecast.pressure_msl.array[current_time.hour]  * 1000.0f,
                                                         (float)press,
                                                         (float)temp/100.0);
            }
            else
            {
                static float start_press = 0.0;
                if(start_press == 0.0)
                {
                    start_press =  (float)press;
                }
                altitude = bmp280::get_height(start_press,
                                              (float)press,
                                              (float)temp/100.0);
            }
            //temp = (temp % 100) < 50 ? (temp / 100) : ((temp / 100) + 1);
            sensorData.weather.temperature = (float)temp / 100.0f;
            sensorData.weather.pressure = press;
            sensorData.altitude = altitude;
            mutex_exit(&sensorDataMutex);
            TRACE_DEBUG(4,TRACE_CORE_0,
                        "Current weather temp:%" PRId32 "C press:%" PRId32 "Pa altitude:%.2fm\n",
                        temp, press, altitude);
        });

        // static char cipgsmloc[20] = {0};
        // CYCLE_UPDATE(sim868::gsm::get_cipgsmloc(cipgsmloc), GPS_FETCH_CYCLE_MS + 100,
        // {},
        // {
        //     mutex_enter_blocking(&sensorDataMutex);
        //     memcpy(sensorData.cipgsmloc, cipgsmloc, sizeof(cipgsmloc));
        //     mutex_exit(&sensorDataMutex);
        //     // std::cout << "Core0: \n:"
        //     //  << "'"
        //     //  << sensorData.clbs << "'\n'"
        //     //  << sensorData.cipgsmloc << "'" << std::endl;
        // });
        // static char clbs[27] = {0};
        // CYCLE_UPDATE(sim868::gsm::get_clbs(clbs), GPS_FETCH_CYCLE_MS + 300,
        // {},
        // {
        //     mutex_enter_blocking(&sensorDataMutex);
        //     memcpy(sensorData.clbs, clbs, sizeof(clbs));
        //     mutex_exit(&sensorDataMutex);
        //     std::cout << "Core0: \n'"
        //      << sensorData.clbs << "'\n'"
        //      << sensorData.cipgsmloc << "'" << std::endl;
        // });
        
        sim868::gps::get_date(current_time);
        mutex_enter_blocking(&sensorDataMutex);
        memcpy(&sensorData.hour, &current_time.hour, sizeof(sensorData.hour));
        //std::cout << "Core0: \n";
        // print(sensorData.forecast.windgusts_10m);
        // std::cout << "len: " << sensorData.forecast.len << std::endl;

        speedDataUpdate(sensorData.speed, sensorData.current_state);
        sensorData.time.t = to_ms_since_boot(get_absolute_time()) / 1000;

        mutex_exit(&sensorDataMutex);

    }
    absolute_time_t frameEnd = get_absolute_time();
    auto frameTimeUs = absolute_time_diff_us(frameStart, frameEnd);
    
    if(fpsToUs(DATA_PER_SECOND) > frameTimeUs)
    {
        // frame took less time
        int64_t timeToSleep = fpsToUs(DATA_PER_SECOND) - frameTimeUs; 
        TRACE_DEBUG(2, TRACE_CORE_0,
                    "frame took %" PRIi64 " max time is %" PRIi64 " sleeping %" PRIi64 "\n",
                    frameTimeUs, fpsToUs(DATA_PER_SECOND), timeToSleep);
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



