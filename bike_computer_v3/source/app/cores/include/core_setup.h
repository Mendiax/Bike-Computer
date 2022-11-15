#ifndef __CORE_SETUP_H__
#define __CORE_SETUP_H__

#include "core1.h"
#include "core0.h"
#include "common_data.hpp"

#include "traces.h"

static inline void core_setup(void);

static inline void core_setup(void)
{
    TRACE_DEBUG(0, TRACE_MAIN, "Init common data\n");
    // sensors_data.current_state = SystemState::TURNED_ON;

    mutex_init(&sensorDataMutex);


    TRACE_DEBUG(0, TRACE_MAIN, "Launch core1\n");
    multicore_launch_core1(core1LaunchThread);

    TRACE_DEBUG(0, TRACE_MAIN, "Start core0 function\n");
    core0_launch_thread();
}

#endif