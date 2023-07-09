
// #------------------------------#
// |          includes            |
// #------------------------------#
// pico includes

// c/c++ includes
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <pthread.h>
#include <unistd.h>

// my includes
#include "pico/time.h"
#include "timers.hpp"
#include "traces.h"

// #------------------------------#
// |           macros             |
// #------------------------------#
#define NO_TIMERS 4
// #------------------------------#
// |    local type definitions    |
// #------------------------------#
struct timer_pthread{
    void *user_data;
    alarm_callback_t callback;
    int64_t delay_us;
    alarm_id_t alarm_id;
};


// #------------------------------#
// | global variables definitions |
// #------------------------------#

// #------------------------------#
// | static variables definitions |
// #------------------------------#
static alarm_pool_t* alarm_pool_default;

static pthread_t alarm_pool_threads[NO_TIMERS];
static volatile bool alarm_pool_threads_taken[NO_TIMERS];

// #------------------------------#
// | static functions declarations|
// #------------------------------#
static void* timer_thread(void* args);
static void* repeating_timer_thread(void* args);
static alarm_id_t get_free_pool(void);
template<typename Alarm_t>
static pthread_t& get_timer_pthread(const Alarm_t * timer_p);

// #------------------------------#
// | global function definitions  |
// #------------------------------#
void mock_cancel_timer(repeating_timer_t * timer_p)
{
    if(timer_p != NULL && alarm_pool_threads_taken[timer_p->alarm_id] == true)
    {
        pthread_cancel(get_timer_pthread(timer_p));
        alarm_pool_threads_taken[timer_p->alarm_id] = false;
        pthread_join(get_timer_pthread(timer_p), NULL);
        *timer_p = repeating_timer{0};
    }
    else {
        TRACE_ABNORMAL(TRACE_HOST, "cancel null or not taken timer %d\n", (int)alarm_pool_threads_taken[timer_p->alarm_id]);
    }
}

bool add_repeating_timer_ms(int32_t delay_ms, repeating_timer_callback_t callback, void *user_data, repeating_timer_t *out)  {
    *out = {delay_ms * 1000, NULL, get_free_pool(), callback, user_data};
    int status = pthread_create(&get_timer_pthread(out), NULL, repeating_timer_thread, out);
    assert(status == 0);
    return true;
}

alarm_id_t add_alarm_in_ms(uint32_t ms, alarm_callback_t callback, void *user_data, bool fire_if_past)
{
    auto timer_p = new timer_pthread;
    *timer_p = timer_pthread{user_data, callback,  (int64_t)(ms * 1000), get_free_pool()};
    int status = pthread_create(&get_timer_pthread(timer_p), NULL, timer_thread, timer_p);
    assert(status == 0);
    return true;
}

// #------------------------------#
// | static functions definitions |
// #------------------------------#
static void* repeating_timer_thread(void* args)
{
    repeating_timer* timer_p= (repeating_timer*)args;
    // TODO handle negative
    sleep_us(std::abs(timer_p->delay_us));
    while (timer_p->callback(timer_p) && alarm_pool_threads[timer_p->alarm_id]) {
        sleep_us(std::abs(timer_p->delay_us));
    }
    alarm_pool_threads_taken[timer_p->alarm_id] = false;
    PRINTF("alarm exited %d", (int)timer_p->alarm_id);
    return NULL;
}

static void* timer_thread(void* args)
{
    timer_pthread* timer_p = (timer_pthread*)args;
    sleep_us(std::abs(timer_p->delay_us));
    int64_t time_to_sleep = 0;
    while ((time_to_sleep = timer_p->callback(timer_p->alarm_id, timer_p->user_data))) {
        // TODO add logic ofr negative return
        sleep_us(std::abs(time_to_sleep));
    }
    alarm_pool_threads_taken[timer_p->alarm_id] = false;
    delete timer_p;
    return NULL;
}

static alarm_id_t get_free_pool(void)
{
    for(alarm_id_t i = 0; i < NO_TIMERS; i++)
    {
        if(alarm_pool_threads_taken[i] == false){
            alarm_pool_threads_taken[i] = true;
            TRACE_DEBUG(0, TRACE_HOST, "added timer at %d\n", i);
            return i;
        }
    }
    TRACE_ABNORMAL(TRACE_HOST, "not enough timers\n");
    assert(false);
    return -1;
}
template<typename Alarm_t>
static pthread_t& get_timer_pthread(const Alarm_t * timer_p)
{
    return alarm_pool_threads[timer_p->alarm_id];
}
