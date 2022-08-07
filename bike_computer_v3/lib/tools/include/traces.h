#ifndef TRACES_H
#define TRACES_H

#include <stdint.h>
#include <pico/sync.h>
#include <stdio.h>

// formats
#define INS_ARR(arr) arr[0], arr[1], arr[2]

#define FORMAT_CREATE_TEMPLATE(form) \
    "[" form ", " form ", " form "]"

#define FORMAT_INT16 "% 3d"
#define FORMAT_INT16_ARR FORMAT_CREATE_TEMPLATE(FORMAT_INT16)

//"[" FORMAT_INT16 "," FORMAT_INT16 "," FORMAT_INT16 "]"
#define FORMAT_FLOAT "% 3.2f"
#define FORMAT_FLOAT_ARR "[" FORMAT_FLOAT "," FORMAT_FLOAT "," FORMAT_FLOAT "]"

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
    TRACE_SPEED,
    BUTTONS,
    TRACE_VIEWS,
    TRACE_DISPLAY_PRINT,
    TRACE_VIEW,
    TRACE_CORE_1,
    TRACE_MPU9250,
    TRACE_BMP280,
    NO_TRACES
};


extern uint32_t tracesOn[];
extern mutex_t tracesMutex;

static inline void tracesSetup()
{
    mutex_init(&tracesMutex);
    // all 0 ids are for abnormal traces and tunrd on on default
    for(size_t i = 0; i < NO_TRACES; i++) 
    {
        TRACES_ON(0, i); 
    }
    TRACES_ON(0, TRACE_MAIN); 
    TRACES_ON_ALL(TRACE_SPEED);

//    TRACES_ON_ALL(TRACE_VIEWS);
    TRACES_ON(0,TRACE_VIEWS);
    TRACES_ON(1,TRACE_VIEWS);
    TRACES_ON(2,TRACE_VIEWS);
    TRACES_ON(3,TRACE_VIEWS);
    TRACES_ON(4,TRACE_VIEWS);
    // 5 6 drawing windows
    // TRACES_ON(5,TRACE_VIEWS);
    // TRACES_ON(6,TRACE_VIEWS);

    TRACES_ON(7,TRACE_VIEWS); // adding new window

    TRACES_ON(1,TRACE_CORE_1); // render time for scree


    //TRACES_ON(0, BUTTONS);
    //TRACES_ON(1, BUTTONS);

    // RACES_ON(1, TRACE_DISPLAY_PRINT); // write string msg
    // RACES_ON(2, TRACE_DISPLAY_PRINT); // wrtie char msg


    TRACES_ON(1, TRACE_MPU9250); // mpu init
    TRACES_ON(2, TRACE_MPU9250);  // reag gyro



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

#define TRACE_ABNORMAL(name, __info, ...) \
    TRACE_DEBUG(0, name, "\x1b[1;31mABNORMAL" __info, ##__VA_ARGS__)

#endif