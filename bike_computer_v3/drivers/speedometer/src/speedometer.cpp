#include <speedometer/speedometer.h>
#include <interrupts/interrupts.hpp>

#include <math.h>
#include <pico/time.h>
#include <hardware/gpio.h>
#include <stdio.h>
#include <string.h>

#if 1
#define DEBUG_SPEED(__info, ...) printf("[DEBUG_SPEED] : " __info, ##__VA_ARGS__)
#else
#define DEBUG_SPEED(__info, ...)
#endif

// time after speed is set to 0 [s]
#define MAX_TIME (WHEEL_SIZE / MIN_SPEED * 1000.0)
#define MAX_TIME_INTERR (WHEEL_SIZE)


 // static variables definitions
static volatile absolute_time_t speed_lastupdate = {0};
//[m/s]
static volatile float speed_velocity = 0.0f;
static volatile unsigned speed_wheelCounter = 0;
// check if last value was read by speed_getSpeed() function
static volatile bool read = true;

// static declarations
static void speed_update();
static absolute_time_t absolute_time_copy_volatile(volatile absolute_time_t* time);


// global definitions
interrupt interruptSpeed = {2, speed_update, GPIO_IRQ_EDGE_FALL};

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
    // TODO if speed is not updated between 2 reads and speed is lower than last speed return speed that it would read if next update occured at time of read
    // (smoother transition into lower speeds)
    // i.e. 20 -> 10 should looks like 20 19 28 ... 10 not 20 10
    absolute_time_t update_us = get_absolute_time();
    absolute_time_t last_update_us; // TODO
    #ifdef NDEBUG
    last_update_us = speed_lastupdate;
    #else
    last_update_us._private_us_since_boot = speed_lastupdate._private_us_since_boot;
    #endif

    if (us_to_ms(absolute_time_diff_us(last_update_us, update_us)) > MAX_TIME)
    {
        read = true;
        return 0.0;
    }
    read = true;
    DEBUG_SPEED("getSpeed : %ul speed : %f\n", to_ms_since_boot(update_us), speed_velocity);
    return speed_velocity;
}


// static definitions
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
static void speed_update()
{
        absolute_time_t update_us = get_absolute_time();
        absolute_time_t last_update_us; // TODO
        last_update_us = absolute_time_copy_volatile(&speed_lastupdate);
        int64_t delta_time = us_to_ms(absolute_time_diff_us(last_update_us, update_us));
        if (delta_time < 50) // less than 10 ms so max speed is not bigger than 787km/h with 27.5 // 100 >
        {
            return;
        }
        speed_wheelCounter++;
        speed_velocity = WHEEL_SIZE / ((float)delta_time / 1000.0);
        // TRACE_SPEED_PRINT("Interrupt time : " + String(update) + " speed : " + String(speed_velocity) + " delta time : " + String(delta_time));
        DEBUG_SPEED("interrupt : %lu speed : %f delta : %lld last : %lu\n", to_ms_since_boot(update_us), speed_velocity, delta_time,  to_ms_since_boot(last_update_us));
        if (read || 1)
        {
            absolute_time_copy_to_volatile(speed_lastupdate, update_us);
            read = false;
        }
}
