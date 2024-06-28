#include <cstddef>
#include <cstdint>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <pico/time.h>
#include <hardware/gpio.h>

#include <speedometer/speedometer.hpp>
#include <interrupts/interrupts.hpp>
#include "utils.hpp"
#include "traces.h"
#if BUILD_FOR_HOST
// simulate timers for cadence sim
#include "timers.hpp"
#endif

#if 0
#define DEBUG_SPEED(__info, ...) printf("[DEBUG_SPEED] : " __info, ##__VA_ARGS__)
#else
#define DEBUG_SPEED(__info, ...)
#endif

#define SPEED_TO_TIME(speed) (wheel_size / speed * 1000.0)

// time after speed is set to 0 [s]
#define MAX_TIME (wheel_size / MIN_SPEED * 1000.0)
#define MAX_TIME_INTERR (wheel_size)


 // static variables definitions
static volatile absolute_time_t speed_lastupdate_prev;
static volatile absolute_time_t speed_lastupdate;

//[m/s]
static volatile float speed_velocity = 0.0f;
static volatile uint_fast64_t speed_wheelCounter = 0;
static volatile uint_fast64_t speed_wheelCounter_total = 0;
static volatile uint_fast64_t speed_total_time = 0;

// check if last value was read by speed_getSpeed() function
static volatile bool dataReady = false;
static bool increment_wheelCounter = 0;
static float wheel_size = 2.0;
static uint8_t no_magnets = 1;

// static declarations
static void speed_update();
//static absolute_time_t absolute_time_copy_volatile(volatile absolute_time_t* time);
static bool repeating_timer_callback(struct repeating_timer *t);
static uint32_t speed_to_ms(float speed_kph);

/*returns last read speed [m/s]*/
static float speed_getSpeed(float speed);
/**
 * @brief calc speed from wheel rotation time
 *
 * @param delta_time in ms
 * @return float speed in m/s
 */
static float speed_velocity_from_delta(int64_t delta_time);

// global definitions
Interrupt interrupt_speed = {PIN_SPEED, speed_update, GPIO_IRQ_EDGE_RISE}; // GPIO_IRQ_EDGE_FALL

float speed_kph_to_mps(float speed_kph)
{
    return speed_kph / 3.6;
}

float speed_mps_to_kmph(float speed_mps)
{
    return speed_mps * 3.6;
}

float speed::get_velocity_kph_raw()
{
    return speed_mps_to_kmph(speed_velocity);
}

/*returns last read speed [m/s]*/
static float speed_getSpeed(float last_speed)
{
    absolute_time_t update_us = get_absolute_time();
    absolute_time_t last_update_us = absolute_time_copy_volatile(&speed_lastupdate);

    int64_t delta_time = us_to_ms(absolute_time_diff_us(last_update_us, update_us));
    float current_speed = 0;

    if(!dataReady) // data is not updated
    {
        auto speed = speed_velocity_from_delta(delta_time);
        if(speed < last_speed)
        {
            last_speed = speed;
        }
    }
    dataReady = false;
    DEBUG_SPEED("getSpeed : %ul speed : %f\n", to_ms_since_boot(update_us), current_speed);
    current_speed = last_speed >= speed_kph_to_mps(MIN_SPEED) ? last_speed : 0.0;
    return current_speed;
}

struct de_accel{
    float speed;
    float accel;
};

int64_t alarm_callback([[maybe_unused]] alarm_id_t id, void* data)
{
    speed_update();
    de_accel* data_ = (de_accel*)data;
    data_->speed += data_->accel * ((float)speed_to_ms(data_->speed) / 1000.0);

    if(data_->speed <= 0.0 || data_->speed >= 99.0)
    {
        return 0;
    }
    int64_t next_alarm = speed_to_ms(data_->speed) * 1000;
    return -1 * next_alarm;
}


void speed_emulate_slowing(float speed, float accel)
{
    auto data = new de_accel();
    *data = {speed, accel};

    // PRINT("adding timer in " << speed_to_ms(speed) << " for " << speed << " w " << accel << "a");
    add_alarm_in_ms(speed_to_ms(speed), alarm_callback, (void*)data, false);
}


void speed_emulate(float speed)
{
    TRACE_DEBUG(0, TRACE_SPEED, "Speed emulate add timer\n");
    static struct repeating_timer timer;
    if(timer.callback != NULL)
        cancel_repeating_timer(&timer);
    add_repeating_timer_ms(-speed_to_ms(speed), repeating_timer_callback, NULL, &timer);
}
// static definitions

// speed in km/h
static uint32_t speed_to_ms(float speed_kph)
{
    const float speed_mps = speed_kph / 3.6;
    // t = s/v
    return (wheel_size / speed_mps) * 1000.0;
}

bool repeating_timer_callback([[maybe_unused]] struct repeating_timer *t) {
    // TRACE_DEBUG(1, TRACE_SPEED, "repeating_timer_callback()\n");
    speed_update();
    return true;
}


static float speed_velocity_from_delta(int64_t delta_time)
{
    const auto speed =  wheel_size / ((double)delta_time / 1000.0) ;
    return speed  / (float)no_magnets;
}
static void speed_update()
{
    absolute_time_t update_us = get_absolute_time();
    absolute_time_t last_update_us = absolute_time_copy_volatile(&speed_lastupdate);

    int64_t delta_time = us_to_ms(absolute_time_diff_us(last_update_us, update_us));
    if (delta_time < speed_to_ms(99.0)) // max speed is not bigger than 99km/h
    {
        return;
    }
    speed_wheelCounter += (increment_wheelCounter == 1);
    speed_wheelCounter_total++;
    speed_velocity = speed_velocity_from_delta(delta_time);
    // if speed > MIN add time to counter
    if(speed_velocity >= speed_kph_to_mps(MIN_SPEED))
    {
        speed_total_time += delta_time;
    }
    absolute_time_copy_to_volatile(speed_lastupdate, update_us);
    dataReady = true;
}

float speed::get_time_total(const bool reset)
{
    return get_time_total_s(reset) / 3600.0f;
}

float speed::get_distance_total(const bool reset)
{
    return get_distance_total_m(reset) / 1000.0f;
}

float speed::get_time_total_s(const bool reset)
{
    const float time_s = (float)speed_total_time / 1000.0f;
    if(reset)
    {
        speed_total_time = 0;
    }
    return time_s;
}

float speed::get_distance_total_m(const bool reset)
{
    const float dist = (float)speed_wheelCounter_total * wheel_size;
    if(reset)
        speed_wheelCounter_total = 0;
    return dist;
}

void speed::setup(float wheel_diameter, uint8_t no_magnets)
{
    wheel_size = wheel_diameter;
    ::no_magnets = no_magnets;
}

float speed::get_velocity_kph()
{
    return speed_mps_to_kmph(speed_getSpeed(speed_velocity));
}

float speed::get_distance_m()
{
    return (float)speed_wheelCounter * wheel_size;
}

float speed::kph_to_rpm(float kph)
{
    float rph = (kph * 1000.0f) / wheel_size;
    return rph  / 60.0f;
}

void speed::start()
{
    increment_wheelCounter = true;
}
void speed::stop()
{
    increment_wheelCounter = false;
}

void speed::reset()
{
    stop();
    speed_wheelCounter = 0;
}

