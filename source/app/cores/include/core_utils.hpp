#ifndef CORE_UTILS_HPP
#define CORE_UTILS_HPP

// #-------------------------------#
// |           includes            |
// #-------------------------------#

// c/c++ includes
#include <cstddef>
#include <stdint.h>

// my includes
#include "pico/time.h"
#include "pico/stdlib.h"
#include "utils.hpp"

// #-------------------------------#
// |            macros             |
// #-------------------------------#

/**
 * @brief Execute function every cycle ms until it returns true, then executes additional code after return of true.
 * @param function The function to execute.
 * @param cycle The cycle time in milliseconds.
 * @param code The code to execute after the function returns true.
 */
#define CYCLE_UPDATE_SIMPLE(function, cycle, code) \
    do { \
        static absolute_time_t __last_update; \
        static bool __executed = false; \
        auto __current_time = get_absolute_time(); \
        if (us_to_ms(absolute_time_diff_us(__last_update, __current_time)) > cycle || \
            (to_ms_since_boot(__current_time) < cycle && !__executed)) { \
            if (function) { \
                __executed = true; \
                /* success */ \
                __last_update = __current_time; \
                { \
                    code \
                } \
            } \
        } \
    } while (0)

/**
 * @brief Execute function every cycle ms until it returns true, with a minimum delay between executions.
 * @param function The function to execute.
 * @param cycle The cycle time in milliseconds.
 * @param min_delay The minimum delay in milliseconds between executions.
 * @param code The code to execute after the function returns true.
 */
#define CYCLE_UPDATE_SIMPLE_SLOW_RERUN(function, cycle, min_delay, code) \
    do { \
        static absolute_time_t __last_update; \
        static bool __executed = false; \
        auto __current_time = get_absolute_time(); \
        auto time_since_last = us_to_ms(absolute_time_diff_us(__last_update, __current_time)); \
        if (((time_since_last > cycle) || \
             (to_ms_since_boot(__current_time) < cycle && !__executed)) && \
            time_since_last > min_delay) { \
            if (function) { \
                __executed = true; \
                /* success */ \
                __last_update = __current_time; \
                { \
                    code \
                } \
            } \
        } \
    } while (0)

// /**
//  * @brief Execute evry cycle ms, does not execute if try to rerun too fast
//  */
/*#define CYCLE_UPDATE(function, force, cycle, pre_code, code) \
     do{ \
         static absolute_time_t __last_update; \
         static bool __executed = false; \
         auto __current_time = get_absolute_time(); \
         if( (force) || \
             us_to_ms(absolute_time_diff_us(__last_update, __current_time)) >  cycle || \
             (to_ms_since_boot(__current_time) < cycle && !__executed) ) \
         { \
             { \
                 pre_code \
             } \
             if(function) \
             { \
                 __executed = true; \
                 __last_update = __current_time; \
                 { \
                     code \
                 } \
             } \
         } \
     } while(0)
*/

// #-------------------------------#
// | global types declarations     |
// #-------------------------------#

// No global types declared in this file

// #-------------------------------#
// | global variables declarations |
// #-------------------------------#

// No global variables declared in this file

// #-------------------------------#
// | global function declarations  |
// #-------------------------------#

/**
 * @brief Converts frames per second (fps) to microseconds (us).
 * @tparam T The type of the fps value.
 * @param fps The frames per second value to convert.
 * @return The equivalent time in microseconds.
 */
template <typename T>
static inline constexpr int64_t fpsToUs(T fps);

/**
 * @brief Checks if the given absolute time is zero.
 * @param time The absolute time to check.
 * @return true if the absolute time is zero, false otherwise.
 */
static inline bool is_absolute_time_zero(absolute_time_t time);

// #-------------------------------#
// |  global function definitions  |
// #-------------------------------#

template <typename T>
static inline constexpr int64_t fpsToUs(T fps)
{
    return (1000 * 1000) / (int64_t)fps;
}

static inline bool is_absolute_time_zero(absolute_time_t time)
{
#ifdef NDEBUG
    return time == 0;
#else
    return time._private_us_since_boot == 0;
#endif
}

#endif // CORE_UTILS_HPP


