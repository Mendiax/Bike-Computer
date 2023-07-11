#ifndef __CORE_SETUP_H__
#define __CORE_SETUP_H__

#include "display_actor.hpp"
#include "data_actor.hpp"


#include "traces.h"

#include <pico/multicore.h>
#include <thread>



void core1LaunchThread(void);


void core_setup(void)
{
    TRACE_DEBUG(0, TRACE_MAIN, "Init common data\n");

    auto data_actor = Data_Actor::get_instance();
    TRACE_DEBUG(0, TRACE_MAIN, "Launch core1\n");

    #ifdef BUILD_FOR_PICO
    multicore_launch_core1(core1LaunchThread);
    #else
    auto core1 = new std::thread(core1LaunchThread);
    #endif

    TRACE_DEBUG(0, TRACE_MAIN, "Start core0 function\n");
    data_actor.run_thread();
}

void core1LaunchThread(void)
{
    auto display_actor = Display_Actor::get_instance();
    display_actor.run_thread();
}

#endif