#include "cadence/cadence.hpp"
#include <interrupts/interrupts.hpp>
#include "traces.h"
#include "utils.hpp"


#include <pico/time.h>
#include <hardware/gpio.h>
#include <pico/sync.h>

#include <math.h>
#include <stdio.h>
#include <string.h>



 // static variables definitions
static volatile absolute_time_t speed_lastupdate_prev;
static volatile absolute_time_t speed_lastupdate;

//[m/s]
static volatile float current_cadence = 0.0f;
// check if last value was read by speed_getSpeed() function
static volatile bool dataReady = false;

// static declarations
static void cadence_update();

static bool repeating_timer_callback(struct repeating_timer *t);

static constexpr uint16_t cadence_to_ms(float speed);
static constexpr float cadence_from_delta(float delta_time);
template<typename T>
static constexpr T rps_to_rpm(T rps);

// global definitions
interrupt interrupt_cadence = {PIN_CADENCE, cadence_update, GPIO_IRQ_EDGE_FALL}; // GPIO_IRQ_EDGE_FALL GPIO_IRQ_EDGE_RISE

#define MAX_TIME (cadence_to_ms(MIN_CADENCE))

/*returns last read speed [m/s]*/
float cadence::get_cadence()
{
    // TRACE_DEBUG(3, TRACE_CADENCE, "current_cadence: %f\n", current_cadence);
    // return current_cadence;

    absolute_time_t update_us = get_absolute_time();
    absolute_time_t last_update_us = absolute_time_copy_volatile(&speed_lastupdate);

    // copy to local buffer global
    auto current_cadence = ::current_cadence;

    if(!dataReady) // data is not updated
    {
        int64_t delta_time = us_to_ms(absolute_time_diff_us(last_update_us, update_us));
        auto cadence = cadence_from_delta(delta_time);
        if(cadence < current_cadence)
        {
            current_cadence = cadence;
        }
    }
    // data is ready
    dataReady = false;
    current_cadence = current_cadence >= MIN_CADENCE ? current_cadence : 0.0f;

    // if (us_to_ms(absolute_time_diff_us(last_update_us, update_us)) > MAX_TIME)
    // {
    //     return 0.0;
    // }
    //DEBUG_SPEED("getSpeed : %ul cadence : %f\n", to_ms_since_boot(update_us), current_cadence);
    TRACE_DEBUG(3, TRACE_CADENCE, "current_cadence: %f\n", current_cadence);

    return current_cadence;
}

void cadence::emulate(float cadence)
{
    TRACE_DEBUG(1, TRACE_CADENCE, "Speed emulate add timer\n");
    static struct repeating_timer timer;
    add_repeating_timer_ms(-cadence_to_ms(cadence), repeating_timer_callback, NULL, &timer);
}

void cadence::speedData_update(SpeedData& speed_data)
{

}


// static definitions


// speed in km/h
constexpr static uint16_t cadence_to_ms(float cadence)
{
    // rpm -> ms/r
    double rps = (cadence / 60.0);
    double ms_per_rot = 1000.0/rps;
    return ms_per_rot;
}


bool repeating_timer_callback(struct repeating_timer *t) {
    cadence_update();
    return true;
}

template<typename T>
static constexpr T rps_to_rpm(T rps)
{
    // rps = x/s
    return rps * (T)60;
}

static constexpr float cadence_from_delta(float delta_time)
{
    double rps = 1000.0 / (double)delta_time;
    return rps_to_rpm(rps);
}
static void cadence_update()
{
        absolute_time_t update_us = get_absolute_time();
        absolute_time_t last_update_us = absolute_time_copy_volatile(&speed_lastupdate);

        int64_t delta_time = us_to_ms(absolute_time_diff_us(last_update_us, update_us));
        if (delta_time < cadence_to_ms(200.0))
        {
            //TRACE_ABNORMAL(TRACE_CADENCE, "too fast cadence updates delta time = %lld\n", delta_time);
            return;
        }

        //TRACE_DEBUG(2, TRACE_CADENCE, "Delta %" PRId64 "\n", delta_time);
        current_cadence = cadence_from_delta(delta_time);
        //DEBUG_SPEED("interrupt : %lu speed : %f delta : %lld last : %lu\n", to_ms_since_boot(update_us), current_cadence, delta_time,  to_ms_since_boot(last_update_us));
        absolute_time_copy_to_volatile(speed_lastupdate, update_us);
        dataReady = true;
}
