#include <speedometer/speedometer.hpp>
#include <interrupts/interrupts.hpp>

#include <math.h>
#include <pico/time.h>
#include <hardware/gpio.h>
#include <stdio.h>
#include <string.h>

#include "traces.h"

#define SIM_INPUT // nie działa xd

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
static volatile absolute_time_t speed_lastupdate = {0};
//[m/s]
static volatile float speed_velocity = 0.0f;
static volatile unsigned speed_wheelCounter = 0;
// check if last value was read by speed_getSpeed() function
static volatile bool dataReady = false;

// static declarations
static void speed_update();
static absolute_time_t absolute_time_copy_volatile(volatile absolute_time_t* time);
static bool repeating_timer_callback(struct repeating_timer *t);
static constexpr uint16_t speed_to_ms(uint16_t speed);
static float speed_velocity_from_delta(float delta_time);

// global definitions
interrupt interruptSpeed = {PIN_SPEED, speed_update, GPIO_IRQ_EDGE_RISE}; // GPIO_IRQ_EDGE_FALL

float speed_mps_to_kmph(float speed_mps)
{
    return speed_mps * 3.6;
}
unsigned speed_getDistance()
{
    return floor((double)speed_wheelCounter * WHEEL_SIZE);
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
            return speed_velocity;
        }
        else
        {
            return speed_velocity;
        }
    } 

    if (us_to_ms(absolute_time_diff_us(last_update_us, update_us)) > MAX_TIME)
    {
        dataReady = false;
        return 0.0;
    }
    dataReady = false;
    DEBUG_SPEED("getSpeed : %ul speed : %f\n", to_ms_since_boot(update_us), speed_velocity);
    return speed_velocity;
}
void speed_emulate(int16_t speed)
{
    TRACE_DEBUG(0, TRACE_SPEED, "Speed emulate add timer\n");
    #ifdef SIM_INPUT
    static struct repeating_timer timer;
    add_repeating_timer_ms(-speed_to_ms(speed), repeating_timer_callback, NULL, &timer);
    #endif
}

void speedDataUpdate(SpeedData& speedData)
{
    static uint64_t driveStartTime;
    if(speed_getDistance() > 0)
        speedData.driveTime = (to_ms_since_boot(get_absolute_time()) / 1000) - driveStartTime;
    else
        driveStartTime = (to_ms_since_boot(get_absolute_time()) / 1000);
    // get data for speed and distance
    speedData.velocity = speed_mps_to_kmph(speed_getSpeed());
    speedData.distance = speed_getDistance() / 1000;
    speedData.distanceDec = (speed_getDistance() / 100) % 100;

    // do some calculations
    speedData.velocityMax = std::fmax(speedData.velocityMax, speedData.velocity);
    speedData.avgGlobal = ((double)speed_getDistance() /(double)speedData.driveTime) * 3.6;

    
    // update buffer
    // uint8_t bufferVelocity = (uint8_t)velocity;
    // ring_buffer_push_overwrite(speedData.speedBuffer, (char *)&bufferVelocity);
}

void speedDataInit(SpeedData& speedData)
{
    memset(&speedData, 0, sizeof(speedData));
    // speedData.speedBuffer = 
    //     ring_buffer_create(sizeof(uint8_t), 100);
}


// static definitions

// speed in km/h
constexpr static uint16_t speed_to_ms(uint16_t speed_kph)
{
    const float speed_mps = speed_kph / 3.6;
    // t = s/v
    return WHEEL_SIZE / speed_mps * 1000.0;
}

bool repeating_timer_callback(struct repeating_timer *t) {
    speed_update();
    return true;
}

static absolute_time_t absolute_time_copy_volatile(volatile absolute_time_t* time)
{
    #ifdef NDEBUG
    return *time;
    #else
    return {time->_private_us_since_boot};
    #endif
}
static void absolute_time_copy_to_volatile(volatile absolute_time_t& time, absolute_time_t timeToCopy)
{
    #ifdef NDEBUG
    time = timeToCopy;
    #else
    time._private_us_since_boot = timeToCopy._private_us_since_boot;
    #endif
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
        if (delta_time < 10) // less than 10 ms so max speed is not bigger than 787km/h with 27.5 // 100 >
        {
            TRACE_ABNORMAL(TRACE_SPEED, "too fast speed updates delta time = %lld\n", delta_time);
            return;
        }
        speed_wheelCounter++;
        speed_velocity = speed_velocity_from_delta(delta_time);
        DEBUG_SPEED("interrupt : %lu speed : %f delta : %lld last : %lu\n", to_ms_since_boot(update_us), speed_velocity, delta_time,  to_ms_since_boot(last_update_us));
        absolute_time_copy_to_volatile(speed_lastupdate, update_us);
        dataReady = true;
}
