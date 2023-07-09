#include "pico/time.h"
// #include <pico/mutex.h>
#include "mock_mutex.hpp"
#include <pico/multicore.h>
// #include <pico/pico_host_sdl.h>

#include <iostream>

#include "pico_test.hpp"
#include "traces.h"

#include <cassert>
#include <cmath>
#include <stdio.h>
#include <pico/stdlib.h>
#include <thread>

#define ITERATIONS 10000000

static mutex mutex_queue;

struct Params
{
    int val;
    bool core1_finished;
};

void iterate(void* args)
{
    auto param = (Params*) args;

    for(int i = 0; i < ITERATIONS; i++)
    {
        mutex_enter_blocking(&mutex_queue);
        param->val++;
        mutex_exit(&mutex_queue);
    }

}

Params params{0, 0};
void core1_kernel()
{
    iterate(&params);
    PRINT("Core1 finished");
    params.core1_finished = true;
    while (1) {
        sleep_ms(1000);
    }
}

TEST_CASE_SUB_FUNCTION_DEF(mutex_test)
{
    mutex_init(&mutex_queue);
    PRINT("multicore_launch_core1");

    multicore_launch_core1(core1_kernel);
    PRINT("Core0 starting");

    iterate(&params);
    PRINT("Core0 finished");


    while(params.core1_finished == false)
    {
        sleep_ms(100);
    }

    PICO_TEST_ASSERT_VERBAL(params.val == 2*ITERATIONS, "Iter: %d", params.val);
}


TEST_CASE_FUNCTION(tc_actors)
{
    TEST_CASE_SUB_FUNCTION(mutex_test);
}


extern "C"
{
    int main()
    {
        traces_init();
        return tc_actors().asserts_failed;
    }
}