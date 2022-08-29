#ifndef CORE_UTILS_HPP
#define CORE_UTILS_HPP
#include <stdint.h>
#include "pico/time.h"
#include "pico/stdlib.h"


#define GSM_ON

#ifdef GSM_ON
#define CYCLE_UPDATE(function, force, cycle, pre_code, code) \
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
                /* success */ \
                __last_update = __current_time; \
                { \
                    code \
                } \
            } \
        } \
    } while(0)

#else
#define CYCLE_UPDATE(function, force, cycle, pre_code, code)
#endif

template <typename T>
static inline constexpr int64_t fpsToUs(T fps)
{
    return (1000 * 1000)/(int64_t)fps;
}

static inline bool is_absolute_time_zero(absolute_time_t time)
{
    #ifdef NDEBUG
    return time == 0;
    #else
    return time._private_us_since_boot == 0;
    #endif
}

#endif
