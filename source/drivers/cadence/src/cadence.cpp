#include "cadence/cadence.hpp"
#include <interrupts/interrupts.hpp>
#include "traces.h"
#include "utils.hpp"

#if BUILD_FOR_HOST
// simulate timers for cadence sim
#include "timers.hpp"
#endif
#include <pico/time.h>
#include <hardware/gpio.h>

#include <math.h>
#include <stdio.h>
#include <string.h>

 // static variables definitions
static volatile absolute_time_t speed_lastupdate_prev;
static volatile absolute_time_t speed_lastupdate;

//[m/s]
static volatile float current_cadence = 0.0f;
// check if last value was read by cadence_update() function
static volatile bool data_ready = false;
static uint8_t no_magnets = 1;


// static declarations
static void cadence_update();

static bool repeating_timer_callback(struct repeating_timer *t);

static constexpr uint16_t cadence_to_ms(float speed);
static float cadence_from_delta(float delta_time);
template<typename T>
static constexpr T rps_to_rpm(T rps);

// global definitions
Interrupt interrupt_cadence = {PIN_CADENCE, cadence_update, GPIO_IRQ_EDGE_FALL};

#define MAX_TIME (cadence_to_ms(MIN_CADENCE))

void cadence::setup(uint8_t no_magnets) {
    ::no_magnets = no_magnets;
}

/*returns last read speed [m/s]*/
float cadence::get_cadence()
{
    const absolute_time_t update_us = get_absolute_time();
    const absolute_time_t last_update_us = absolute_time_copy_volatile(&speed_lastupdate);

    // copy to local buffer global
    auto current_cadence = ::current_cadence;

    if(!data_ready) // data is not updated
    {
        const int64_t delta_time = us_to_ms(absolute_time_diff_us(last_update_us, update_us));
        const auto cadence = cadence_from_delta(delta_time);
        if(cadence < current_cadence)
        {
            current_cadence = cadence;
        }
    }
    // data is ready
    data_ready = false;
    current_cadence = current_cadence >= MIN_CADENCE ? current_cadence : 0.0f;

    TRACE_DEBUG(3, TRACE_CADENCE, "current_cadence: %f\n", current_cadence);

    return current_cadence;
}

void cadence::emulate(const float cadence)
{
    TRACE_DEBUG(1, TRACE_CADENCE, "Speed emulate add timer\n");
    static struct repeating_timer timer;
    cancel_repeating_timer(&timer);
    add_repeating_timer_ms(-cadence_to_ms(cadence), repeating_timer_callback, NULL, &timer);
}


// static definitions


// speed in km/h
constexpr static uint16_t cadence_to_ms(const float cadence)
{
    // rpm -> ms/r
    const double rps = (cadence / 60.0);
    const double ms_per_rot = 1000.0/rps;
    return ms_per_rot;
}


bool repeating_timer_callback([[maybe_unused]] struct repeating_timer *t) {
    cadence_update();
    return true;
}

template<typename T>
static constexpr T rps_to_rpm(T rps)
{
    // rps = x/s
    return rps * (T)60;
}

static float cadence_from_delta(const float delta_time)
{
    const double rps = 1000.0 / (double)delta_time;
    return rps_to_rpm(rps) / (float)no_magnets;
}
static void cadence_update()
{
    const absolute_time_t update_us = get_absolute_time();
    const absolute_time_t last_update_us = absolute_time_copy_volatile(&speed_lastupdate);

    int64_t delta_time = us_to_ms(absolute_time_diff_us(last_update_us, update_us));
    if (delta_time < cadence_to_ms(200.0))
    {
        return;
    }

    current_cadence = cadence_from_delta(delta_time);
    absolute_time_copy_to_volatile(speed_lastupdate, update_us);
    data_ready = true;
}
