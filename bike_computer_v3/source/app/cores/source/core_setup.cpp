#ifndef __CORE_SETUP_H__
#define __CORE_SETUP_H__

#include "display_actor.hpp"
#include "data_actor.hpp"
#include "common_data.hpp"

#include "traces.h"

#include <pico/multicore.h>

#define QUEUE_LEN 2


void core1LaunchThread(void);

static Data_Queue<actors_common::Packet>* pc_queue;

void core_setup(void)
{
    TRACE_DEBUG(0, TRACE_MAIN, "Init common data\n");

    pc_queue = new Data_Queue<actors_common::Packet>(QUEUE_LEN);


    auto data_actor = Data_Actor::get_instance();
    data_actor.set_pc_queue(pc_queue);
    TRACE_DEBUG(0, TRACE_MAIN, "Launch core1\n");
    multicore_launch_core1(core1LaunchThread);

    TRACE_DEBUG(0, TRACE_MAIN, "Start core0 function\n");
    data_actor.run_thread();
}

void core1LaunchThread(void)
{
    auto display_actor = Display_Actor::get_instance();
    // display_actor.set_pc_queue(pc_queue);
    display_actor.run_thread();
}

#endif