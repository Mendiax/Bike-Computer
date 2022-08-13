// pico includes
#include "pico/stdlib.h"

// c/c++ includes
#include <tuple>
#include <inttypes.h>
#include <string.h>

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

#define DATA_PER_SECOND 100
#define BAT_LEV_CYCLE_MS (30*1000)
#define GPS_FETCH_CYCLE_MS (10*1000)


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


#define CYCLE_UPDATE(function, cycle, code) \
    do{ \
        static absolute_time_t __last_update; \
        auto __current_time = get_absolute_time(); \
        if(us_to_ms(absolute_time_diff_us(__last_update, __current_time)) >  cycle || is_absolute_time_zero(__last_update)) \
        { \
            if(function) \
            { \
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
                        TRACE_DEBUG(1,TRACE_CORE_0, "bat info %d,%" PRIu8 ",%" PRIu16 "\n", is_charging, bat_lev, voltage);
                    }
                    );

        static float speed;
        static float latitude;
        static float longitude;
        static float msl;

        static TimeS current_time;

        // keep gps on
        if(sim868::gps::get_gps_state() == GpsState::OFF)
        {
            sim868::gps::turn_on();
        }
        CYCLE_UPDATE(sim868::gps::fetch_data(), GPS_FETCH_CYCLE_MS,
                    {
                        sim868::gps::get_speed(speed);
                        sim868::gps::get_position(latitude, longitude);
                        //sim868::gps::get_date(current_time);
                        sim868::gps::get_msl(msl);
                        // speed = 69.3;
                        // latitude = 51.104877842621484;
                        // longitude = 17.031670433667298;
                        // msl = 123.34;

                        time_print(current_time);
                        TRACE_DEBUG(3,TRACE_CORE_0, "gps speed %.1f, pos [%.5f,%.5f] date year = %" PRIu16 "\n", speed, latitude, longitude, current_time.year);
                    }
                    );
        mutex_enter_blocking(&sensorDataMutex);

        sim868::gps::get_date(current_time);
        memcpy(&sensorData.hour, &current_time.hour, sizeof(sensorData.hour));
        memcpy(&sensorData.date, &current_time.year, sizeof(sensorData.date));

        if(sensorData.current_state != SystemState::PAUSED)
        {
            speedDataUpdate(sensorData.speed);
            sensorData.time.t = to_ms_since_boot(get_absolute_time()) / 1000;
        }

        sensorData.lipo.is_charging = is_charging;
        sensorData.lipo.level = bat_lev;


        sensorData.gps_data.speed = speed;
        sensorData.gps_data.lat = latitude;
        sensorData.gps_data.lon = longitude;
        sensorData.gps_data.msl = msl;


        mutex_exit(&sensorDataMutex);
    }
        
    absolute_time_t frameEnd = get_absolute_time();
    auto frameTimeUs = absolute_time_diff_us(frameStart, frameEnd);
    if(fpsToUs(DATA_PER_SECOND) > frameTimeUs)
    {
        // frame took less time
        int64_t timeToSleep = fpsToUs(DATA_PER_SECOND) - frameTimeUs; 
        sleep_us(timeToSleep);
        TRACE_DEBUG(2, TRACE_CORE_0, "frame took %" PRIi64 " max time is %" PRIi64 " sleeping %" PRIi64 "\n",frameTimeUs, fpsToUs(DATA_PER_SECOND), timeToSleep);
    }
    else
    {
        int64_t timeToSleep = fpsToUs(DATA_PER_SECOND) - frameTimeUs; 
        TRACE_ABNORMAL(TRACE_CORE_0, "frame took %" PRIi64 " should be %" PRIi64 " delta %" PRIi64 "\n",frameTimeUs, fpsToUs(DATA_PER_SECOND), timeToSleep);
    }

    return 1;
}



