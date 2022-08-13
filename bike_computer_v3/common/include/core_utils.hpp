#ifndef CORE_UTILS_HPP
#define CORE_UTILS_HPP
#include <stdint.h>
#include "pico/time.h"

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
