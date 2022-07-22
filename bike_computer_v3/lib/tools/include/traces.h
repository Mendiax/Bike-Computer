#ifndef TRACES_H
#define TRACES_H

#include <stdint.h>
#include <pico/sync.h>

// #if 1
// 	#define DEBUG_OLED(__info,...) printf("[DEBUG_OLED] : " __info,##__VA_ARGS__)
// #else
// 	#define DEBUG_OLED(__info,...)  
// #endif

#define TRACES_ON(id, name) \
        tracesOn[name] |= (1 << id)
#define TRACES_ON_ALL(name) \
        tracesOn[name] = ~0UL

/**
 * @brief Traces types
 * Each trace must have been assigned to one type
 * Each type can hold 32 traces and those do not need to be assigned
 * but there is no checks if 2 traces hold the same id, so few traces can
 * share the same id
 */
enum tracesE{
    TRACE_MAIN,
    BUTTONS,
    NO_TRACES
};


extern uint32_t tracesOn[];
extern mutex_t tracesMutex;

static inline void tracesSetup()
{
    mutex_init(&tracesMutex);
    TRACES_ON(0, TRACE_MAIN); 
    //TRACES_ON(0, BUTTONS);
    //TRACES_ON(1, BUTTONS);

}

namespace utility {

    template <typename T, size_t S>
    inline constexpr size_t get_file_name_offset(const T (& str)[S], size_t i = S - 1)
    {
        return (str[i] == '/' || str[i] == '\\') ? i + 1 : (i > 0 ? get_file_name_offset(str, i - 1) : 0);
    }

    template <typename T>
    inline constexpr size_t get_file_name_offset(T (& str)[1])
    {
        return 0;
    }

    template <typename T, T v>
    struct const_expr_value
    {
        static constexpr const T value = v;
    };

}

#define UTILITY_CONST_EXPR_VALUE(exp) ::utility::const_expr_value<decltype(exp), exp>::value


#define TRACE_DEBUG(id, name, __info,...) \
    do{\
    if (tracesOn[name] & (1 << id))\
    {\
        mutex_enter_blocking(&tracesMutex); \
        printf("[" #name ".%2u]<%s:%d> " __info,\
            id,\
            &__FILE__[UTILITY_CONST_EXPR_VALUE(utility::get_file_name_offset(__FILE__))],\
            __LINE__,\
            ##__VA_ARGS__);\
        mutex_exit(&tracesMutex); \
    }\
    }while(0)

#endif