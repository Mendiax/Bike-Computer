#include <speedometer/speedometer.hpp>
#include <interrupts/interrupts.hpp>
#include "common_utils.hpp"

#include <math.h>
#include <pico/time.h>
#include <hardware/gpio.h>
#include <stdio.h>
#include <string.h>

#include "traces.h"

#if 0
#define DEBUG_SPEED(__info, ...) printf("[DEBUG_SPEED] : " __info, ##__VA_ARGS__)
#else
#define DEBUG_SPEED(__info, ...)
#endif

#define SPEED_TO_TIME(speed) (WHEEL_SIZE / speed * 1000.0)

// time after speed is set to 0 [s]
#define MAX_TIME (WHEEL_SIZE / MIN_SPEED * 1000.0)
#define MAX_TIME_INTERR (WHEEL_SIZE)


 // static variables definitions
static volatile absolute_time_t speed_lastupdate_prev;
static volatile absolute_time_t speed_lastupdate;

//[m/s]
static volatile float speed_velocity = 0.0f;
static volatile uint_fast64_t speed_wheelCounter = 0;
// check if last value was read by speed_getSpeed() function
static volatile bool dataReady = false;
static bool increment_wheelCounter = 0;
static uint16_t* distance_counter_p;

// static declarations
static void speed_update();
//static absolute_time_t absolute_time_copy_volatile(volatile absolute_time_t* time);
static bool repeating_timer_callback(struct repeating_timer *t);
static constexpr uint16_t speed_to_ms(float speed);
static float speed_velocity_from_delta(float delta_time);

// global definitions
interrupt interruptSpeed = {PIN_SPEED, speed_update, GPIO_IRQ_EDGE_RISE}; // GPIO_IRQ_EDGE_FALL

float speed_kph_to_mps(float speed_kph)
{
    return speed_kph / 3.6;
}

float speed_mps_to_kmph(float speed_mps)
{
    return speed_mps * 3.6;
}
float speed_getDistance()
{
    return (double)speed_wheelCounter * WHEEL_SIZE;
}

/*returns last read speed [m/s]*/
float speed_getSpeed()
{
    // TODO if speed is not updated between 2 reads and speed is lower than last speed return
    // speed that it would read if next update occured at time of read
    // (smoother transition into lower speeds)
    // i.e. 20 -> 10 should looks like 20 19 28 ... 10 not 20 10
    absolute_time_t update_us = get_absolute_time();
    absolute_time_t last_update_us = absolute_time_copy_volatile(&speed_lastupdate);

    if(!dataReady) // data is not updated
    {
        int64_t delta_time = us_to_ms(absolute_time_diff_us(last_update_us, update_us));
        auto speed = speed_velocity_from_delta(delta_time);
        if(speed < speed_velocity)
        {
            speed_velocity = speed;
        }
        speed_velocity = speed_velocity >= speed_kph_to_mps(MIN_SPEED) ? speed_velocity : 0.0;
        return speed_velocity;
    }

    dataReady = false;
    if (us_to_ms(absolute_time_diff_us(last_update_us, update_us)) > MAX_TIME)
    {
        return 0.0;
    }
    DEBUG_SPEED("getSpeed : %ul speed : %f\n", to_ms_since_boot(update_us), speed_velocity);
    return speed_velocity;
}
void speed_emulate(float speed)
{
    TRACE_DEBUG(0, TRACE_SPEED, "Speed emulate add timer\n");
    static struct repeating_timer timer;
    add_repeating_timer_ms(-speed_to_ms(speed), repeating_timer_callback, NULL, &timer);
}
// static definitions

// speed in km/h
constexpr static uint16_t speed_to_ms(float speed_kph)
{
    const float speed_mps = speed_kph / 3.6;
    // t = s/v
    return WHEEL_SIZE / speed_mps * 1000.0;
}

bool repeating_timer_callback(struct repeating_timer *t) {
    speed_update();
    return true;
}


static float speed_velocity_from_delta(float delta_time)
{
    return WHEEL_SIZE / ((float)delta_time / 1000.0);
}
static void speed_update()
{
        absolute_time_t update_us = get_absolute_time();
        absolute_time_t last_update_us = absolute_time_copy_volatile(&speed_lastupdate);

        int64_t delta_time = us_to_ms(absolute_time_diff_us(last_update_us, update_us));
        if (delta_time < speed_to_ms(99.0)) // less than 10 ms so max speed is not bigger than 787km/h with 27.5 // 100 >
        {
            //TRACE_ABNORMAL(TRACE_SPEED, "too fast speed updates delta time = %lld\n", delta_time);
            return;
        }
        //PRINTF("speed delta %" PRId64 "\n", delta_time);
        speed_wheelCounter += (increment_wheelCounter == 1);
        speed_velocity = speed_velocity_from_delta(delta_time);
        //DEBUG_SPEED("interrupt : %lu speed : %f delta : %lld last : %lu\n", to_ms_since_boot(update_us), speed_velocity, delta_time,  to_ms_since_boot(last_update_us));
        absolute_time_copy_to_volatile(speed_lastupdate, update_us);
        dataReady = true;
}

float speed::get_velocity_kph()
{
    return speed_mps_to_kmph(speed_getSpeed());
}

float speed::get_distance_m()
{
    return speed_getDistance();
}

float speed::kph_to_rpm(float kph)
{
    float rph = (kph * 1000.0f) / WHEEL_SIZE;
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

