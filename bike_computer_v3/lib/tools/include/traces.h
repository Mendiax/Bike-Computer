#ifndef TRACES_H
#define TRACES_H

#include <stdint.h>
#include <pico/sync.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include <inttypes.h>

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
    TRACES_ON(3, TRACE_CORE_0); // gps update
    // TRACES_ON(4, TRACE_CORE_0); // BMP280 updatre
    //TRACES_ON(5, TRACE_CORE_0); // gear ratio




    // ==================================================
    //                   CORE 1 TRACES
    // ==================================================
    //TRACES_ON(1,TRACE_CORE_1); // render time for scree
    //TRACES_ON(2,TRACE_CORE_1); // pause btn


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
    //                   MPU9250 TRACES
    // ==================================================
    TRACES_ON(1, TRACE_MPU9250); // mpu init
    //TRACES_ON(2, TRACE_MPU9250);  // reag gyro

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
    TRACES_ON(2, TRACE_BIKE_CONFIG); // min diff gear ratio


    // ==================================================
    //                   SD TRACES
    // ==================================================
    //TRACES_ON(1, TRACE_SD);  // read file size

    // ==================================================
    //                   ACTORS TRACES
    // ==================================================

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



#define TRACE_DEBUG(id, name, __info,...) \
    do{\
    if (tracesOn[name] & (1 << id))\
    {\
        float __time_since_boot = (float)to_ms_since_boot(get_absolute_time()) / 1000.0; \
        PRINTF("[" ESC_GREEN #name ".%u" ESC_RESET "][" ESC_WHITE "%.3fs" ESC_RESET "] " ESC_WHITE __info ESC_RESET, \
            id, \
            __time_since_boot, \
            ##__VA_ARGS__); \
    }\
    }while(0)

#define TRACE_ABNORMAL(name, __info, ...) \
    TRACE_DEBUG(0, name, ESC_RED "ABNORMAL " __info ESC_RESET, ##__VA_ARGS__)

#endif