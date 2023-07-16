#ifndef __GPIO_TIMERS_HPP__
#define __GPIO_TIMERS_HPP__
// #-------------------------------#
// |           includes            |
// #-------------------------------#
// pico includes
#include <pico/time.h>

// c/c++ includes
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

// my includes

// #-------------------------------#
// |            macros             |
// #-------------------------------#
#define cancel_repeating_timer(timer) mock_cancel_timer(timer)
// #-------------------------------#
// | global types declarations     |
// #-------------------------------#

// #-------------------------------#
// | global variables declarations |
// #-------------------------------#

// #-------------------------------#
// | global function declarations  |
// #-------------------------------#


void mock_cancel_timer(repeating_timer_t * timer_p);

bool add_repeating_timer_ms(int32_t delay_ms, repeating_timer_callback_t callback, void *user_data, repeating_timer_t *out);

alarm_id_t add_alarm_in_ms(uint32_t ms, alarm_callback_t callback, void *user_data, bool fire_if_past);

#endif
