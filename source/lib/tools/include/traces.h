#ifndef TRACES_H
#define TRACES_H

#include <stdint.h>
#ifdef MOCK_MULTICORE
#include "mock_mutex.hpp"
#else
#include <pico/sync.h>
#endif
#include <stdio.h>
#include "pico/stdlib.h"
#include <inttypes.h>
#include <bitset>
#include <iostream>

#define PRINT_BITS(x) do{std::bitset<sizeof(x)*8> __bits = x; std::cout << #x "=" <<__bits << std::endl;}while(0)
#define PRINT_BITS_N(x, n) do{std::bitset<n> __bits = x; std::cout << #x "=" <<__bits << std::endl;}while(0)
#define PRINT_BITS_ARRAY(arr, n) do{\
    std::cout << #arr; \
    for(size_t i = 0; i < n; i++){\
    if(i % 8 == 0){\
        std::cout << std::endl << "\t";\
        } \
        std::bitset<sizeof(*arr) * 8> __bits = arr[i]; \
        std::cout << __bits << " "; \
        }\
     std::cout << std::endl;\
     }while (0)



// formats
#define INS_ARR(arr) arr[0], arr[1], arr[2]

#define FORMAT_CREATE_TEMPLATE(form) \
    "[" form ", " form ", " form "]"

#define FORMAT_INT16 "% 3d"
#define FORMAT_INT16_ARR FORMAT_CREATE_TEMPLATE(FORMAT_INT16)

//"[" FORMAT_INT16 "," FORMAT_INT16 "," FORMAT_INT16 "]"
#define FORMAT_FLOAT "% 3.2f"
#define FORMAT_FLOAT_ARR "[" FORMAT_FLOAT "," FORMAT_FLOAT "," FORMAT_FLOAT "]"

#define TRACES_ON(id, name) \
        tracesOn[name] |= (1 << id)
#define PLOT(id) \
        tracesOn[TRACE_PLOTTER] |= (1 << id)
#define TRACES_ON_ALL(name) \
        tracesOn[name] = ~((uint32_t)0)

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
    TRACE_CADENCE,
    TRACE_BIKE_CONFIG,
    BUTTONS,
    TRACE_VIEWS,
    TRACE_DISPLAY_PRINT,
    TRACE_VIEW,
    TRACE_CORE_0,
    TRACE_CORE_1,
    TRACE_MPU9250,
    TRACE_BMP280,
    TRACE_SIM868,
    TRACE_SD,
    TRACE_ACTOR,
    TRACE_GUI,
    TRACE_PLOTTER,
    TRACE_HOST, // test debug only!
    NO_TRACES
};


extern uint32_t tracesOn[];
extern mutex tracesMutex;

static inline void traces_init()
{
    mutex_init(&tracesMutex);
    // all 0 ids are for abnormal traces and tunrd on on default
    for(size_t i = 0; i < NO_TRACES; i++)
    {
        TRACES_ON(0, i);
    }
    TRACES_ON(0, TRACE_MAIN);
    TRACES_ON_ALL(TRACE_SPEED);

    // ==================================================
    //                   VIEWS TRACES
    // ==================================================
    // TRACES_ON_ALL(TRACE_VIEWS);
    // TRACES_ON(0,TRACE_VIEWS);
    // TRACES_ON(1,TRACE_VIEWS); // split frame
    // TRACES_ON(2,TRACE_VIEWS);
    // TRACES_ON(3,TRACE_VIEWS);
    // TRACES_ON(4,TRACE_VIEWS); // render view
    // 5 6 drawing windows
    // TRACES_ON(5,TRACE_VIEWS);
    // TRACES_ON(6,TRACE_VIEWS);

    // TRACES_ON(7,TRACE_VIEWS); // adding new window

    // ==================================================
    //                   CORE 0 TRACES
    // ==================================================
    TRACES_ON(1,TRACE_CORE_0); // bat info
    // TRACES_ON(2, TRACE_CORE_0); // data update time
    // TRACES_ON(3, TRACE_CORE_0); // gps update
    // TRACES_ON(4, TRACE_CORE_0); // BMP280 update
    // TRACES_ON(5, TRACE_CORE_0); // gear ratio
    TRACES_ON(6, TRACE_CORE_0); // signal strength





    // ==================================================
    //                   CORE 1 TRACES
    // ==================================================
    // TRACES_ON(1,TRACE_CORE_1); // render time for scree
    // TRACES_ON(2,TRACE_CORE_1); // pause btn



    // ==================================================
    //                   BUTTONS TRACES
    // ==================================================
    //TRACES_ON(0, BUTTONS);
    //TRACES_ON(1, BUTTONS);

    // ==================================================
    //                   DISPLAY TRACES
    // ==================================================
    // RACES_ON(1, TRACE_DISPLAY_PRINT); // write string msg
    // RACES_ON(2, TRACE_DISPLAY_PRINT); // wrtie char msg
    // RACES_ON(3, TRACE_DISPLAY_PRINT); // center space


    // ==================================================
    //                   BMP280 TRACES
    // ==================================================
    // TRACES_ON(1, TRACE_BMP280);
    // PLOT(1);


    // ==================================================
    //                   MPU9250 TRACES
    // ==================================================
    TRACES_ON(1, TRACE_MPU9250); // mpu init
    TRACES_ON(2, TRACE_MPU9250);  // reag gyro
    // TRACES_ON(3, TRACE_MPU9250);  // reag mag
    // TRACES_ON(4, TRACE_MPU9250);  // reag accel



    // ==================================================
    //                   SIM 868 TRACES
    // ==================================================
    //TRACES_ON(1, TRACE_SIM868);  // send request log long
    // TRACES_ON(2, TRACE_SIM868);  // send_request log


    // ==================================================
    //                   CADENCE TRACES
    // ==================================================
    //TRACES_ON(1, TRACE_CADENCE); // emulate
    //TRACES_ON(2, TRACE_CADENCE); // sensor update
    //TRACES_ON(3, TRACE_CADENCE); // get cadence

    // ==================================================
    //                   CADENCE TRACES
    // ==================================================
    TRACES_ON(1, TRACE_BIKE_CONFIG); // to str
    // TRACES_ON(2, TRACE_BIKE_CONFIG); // min diff gear ratio
    // TRACES_ON(3, TRACE_BIKE_CONFIG); // add gear usage



    // ==================================================
    //                   SD TRACES
    // ==================================================
    TRACES_ON(1, TRACE_SD);  // read file size
    TRACES_ON(2, TRACE_SD);  // SD mount


    // ==================================================
    //                   ACTORS TRACES
    // ==================================================
    // TRACES_ON(1, TRACE_ACTOR);

    // ==================================================
    //                   GUI TRACES
    // ==================================================
    TRACES_ON(1, TRACE_GUI); // history log data update

}

#define ESC "\x1b"

#define ESC_RESET ESC "[0m"

#define ESC_RED ESC "[1;31m"
#define ESC_GREEN ESC "[1;32m"
#define ESC_WHITE ESC "[1;37m"




#define PRINTF(format, ...) \
    do{ \
        mutex_enter_blocking(&tracesMutex); \
        printf(format, ##__VA_ARGS__); \
        mutex_exit(&tracesMutex); \
    } while (0)

#define PRINT(...) \
    std::cout << __VA_ARGS__ << std::endl

#define TRACE_PLOT(id, name, value) \
    do{\
    if (tracesOn[TRACE_PLOTTER] & (1 << id))\
    {\
        PRINT("[PLOT <" name ">] " << value); \
    }\
    }while(0)

#define TRACE_DEBUG(id, name, __info,...) \
    do{\
    if (tracesOn[name] & (1 << id))\
    {\
        float __time_since_boot = (float)to_ms_since_boot(get_absolute_time()) / 1000.0; \
        PRINTF(ESC_WHITE "%8.3f: " ESC_RESET "[" ESC_GREEN #name ".%u:%d" ESC_RESET "] " ESC_WHITE __info ESC_RESET, \
            __time_since_boot, \
            id, \
            (int)__LINE__, \
            ##__VA_ARGS__); \
    }\
    }while(0)

#define TRACE_ABNORMAL(name, __info, ...) \
    TRACE_DEBUG(0, name, ESC_RED "ABNORMAL " __info ESC_RESET,  ##__VA_ARGS__)

#endif