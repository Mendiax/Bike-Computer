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
static volatile absolute_time_t speed_lastupdate_prev;
static volatile absolute_time_t speed_lastupdate;

//[m/s]
static volatile float speed_velocity = 0.0f;
static volatile uint_fast64_t speed_wheelCounter = 0;
// check if last value was read by speed_getSpeed() function
static volatile bool dataReady = false;
static bool increment_wheelCounter = 0;

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

void speedDataUpdate(SpeedData& speed_data, SystemState& state)
{
    // TODO refactor xd
    static uint32_t time_last_update;
    static uint32_t drive_start_time = 0;


    auto time_ms = to_ms_since_boot(get_absolute_time());
    uint32_t time_delta = time_ms - time_last_update;
    time_last_update = time_ms;

    // check for start
    if(state == SystemState::AUTOSTART && speed_getDistance() > 0)
    {
        state = SystemState::RUNNING;

        // reset data
        speed_wheelCounter = 0;
        drive_start_time = time_ms;
        speed_data.stop_time = 0;
    }

    switch (state)
    {
    case SystemState::RUNNING:
        speed_data.drive_time = ((time_ms - drive_start_time) - speed_data.stop_time) / 1000 ;
        increment_wheelCounter = 1;
        break;
    case SystemState::PAUSED:
        increment_wheelCounter = 0;
        speed_data.stop_time += time_delta;
        break;
    case SystemState::AUTOSTART:
        increment_wheelCounter = 1;
    case SystemState::CHARGING:
    case SystemState::TURNED_ON:
        drive_start_time = 0;
        speed_data.stop_time = 0;
        break;
    case SystemState::STOPPED:
        increment_wheelCounter = 0;
        // TODO
    default:
        break;
    }


    if(state == SystemState::RUNNING)
    {
        auto distance_m = speed_getDistance(); 
        // get data for speed and distance
        speed_data.velocity = speed_mps_to_kmph(speed_getSpeed());
        speed_data.distance = distance_m / 1000.0;
        speed_data.distanceDec = (uint64_t)(distance_m / 100.0) % 100;

        // do some calculations
        speed_data.velocityMax = std::fmax(speed_data.velocityMax, speed_data.velocity);
        //PRINTF("max=%f\n", speed_data.velocityMax);
        
        double drive_time_s = ((double)speed_data.drive_time);
        if(drive_time_s > 0.0)
        {
            speed_data.avg = speed_mps_to_kmph((double)distance_m / drive_time_s);
        }
        double drive_global_time_s = ((double)(time_ms - drive_start_time) / 1000.0);
        if(drive_global_time_s > 0.0)
        {
            speed_data.avg_global = speed_mps_to_kmph((double)distance_m / drive_global_time_s);
        }
    }

    
    // update buffer
    // uint8_t bufferVelocity = (uint8_t)velocity;
    // ring_buffer_push_overwrite(speed_data.speedBuffer, (char *)&bufferVelocity);
}

void speedDataInit(SpeedData& speed_data)
{
    memset(&speed_data, 0, sizeof(speed_data));
    // speed_data.speedBuffer = 
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
        if (delta_time < speed_to_ms(99)) // less than 10 ms so max speed is not bigger than 787km/h with 27.5 // 100 >
        {
            TRACE_ABNORMAL(TRACE_SPEED, "too fast speed updates delta time = %lld\n", delta_time);
            return;
        }
        speed_wheelCounter += (increment_wheelCounter == 1);
        speed_velocity = speed_velocity_from_delta(delta_time);
        //DEBUG_SPEED("interrupt : %lu speed : %f delta : %lld last : %lu\n", to_ms_since_boot(update_us), speed_velocity, delta_time,  to_ms_since_boot(last_update_us));
        absolute_time_copy_to_volatile(speed_lastupdate, update_us);
        dataReady = true;
}
