#ifndef CORE_UTILS_HPP
#define CORE_UTILS_HPP
#include <cstddef>
#include <stdint.h>
#include "pico/time.h"
#include "pico/stdlib.h"
#include "pico/sync.h"
#include "utils.hpp"

#define CYCLE_UPDATE_SIMPLE(function, cycle, code) \
    do{ \
        static absolute_time_t __last_update; \
        static bool __executed = false; \
        auto __current_time = get_absolute_time(); \
        if( us_to_ms(absolute_time_diff_us(__last_update, __current_time)) >  cycle || \
            (to_ms_since_boot(__current_time) < cycle && !__executed) ) \
        { \
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

/**
 * @brief does not execute if try to rerun too fast
 *
 */
#define CYCLE_UPDATE_SIMPLE_SLOW_RERUN(function, cycle, min_delay, code) \
    do{ \
        static absolute_time_t __last_update; \
        static bool __executed = false; \
        auto __current_time = get_absolute_time(); \
        auto time_since_last = us_to_ms(absolute_time_diff_us(__last_update, __current_time)); \
        if( ((time_since_last >  cycle) || \
             (to_ms_since_boot(__current_time) < cycle && !__executed) \
            ) && \
            time_since_last > min_delay \
            ) \
        { \
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

/**
 * @brief does not execute if try to rerun too fast
 *
 */
#define CYCLE_UPDATE_SLOW_RERUN(function, force, cycle, min_delay, pre_code, code) \
    do{ \
        static absolute_time_t __last_update; \
        static bool __executed = false; \
        auto __current_time = get_absolute_time(); \
        auto time_since_last = us_to_ms(absolute_time_diff_us(__last_update, __current_time)); \
        if( ((force) || \
             (time_since_last >  cycle) || \
             (to_ms_since_boot(__current_time) < cycle && !__executed) \
            ) && \
            time_since_last > min_delay \
            ) \
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

template<typename T>
class Pc_Queue{
    T* buffer;
    size_t no_elements_cur;
    const size_t no_elements_max;
    mutex_t mutex;

    Pc_Queue(size_t size)
    : no_elements_cur{0}, no_elements_max{size}
    {
        buffer = new T[size];
        mutex_init(&mutex);
    }

    inline bool is_full()
    {
        Unique_Mutex umutex(&mutex);
        return no_elements_cur == no_elements_max;
    }

    inline bool is_empty()
    {
        Unique_Mutex umutex(&mutex);
        return no_elements_cur == 0;
    }

    inline void append(const T& elem)
    {
        // if()
    }

    ~Pc_Queue()
    {
        delete [] buffer;
    }
};

#endif
