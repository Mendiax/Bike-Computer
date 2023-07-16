// #include "traces.h"
#include "pico/time.h"
#include "pico_test.hpp"
#include "traces.h"
#include <cassert>
#include <cmath>
#include <stdio.h>
#include <pico/stdlib.h>
#include <speedometer/speedometer.hpp>
#include <iostream>

TEST_CASE_SUB_FUNCTION_DEF(sim_avg_test)
{
    speed::set_wheel(1.0); // small wheel for faster rotation

    {
        const float target_speed = 20.0f;
        speed_emulate(target_speed);
        sleep_ms(500);
        speed::start();
        float curent_speed = speed::get_velocity_kph();
        PICO_TEST_CHECK_VERBAL(curent_speed > 0, "speed: %.2f", curent_speed);

        //reset
        speed::get_distance_total(true);
        speed::get_time_total(true);
        // allow to run
        sleep_ms(1000);

        //read data
        const float dist_m = speed::get_distance_total_m(true);
        const float time_s = speed::get_time_total_s(true);
        speed::stop();
        const float speed = speed_mps_to_kmph(dist_m/time_s);
        PICO_TEST_CHECK_VERBAL(std::abs(speed - target_speed) < 0.5, "speed:%f target:%f", speed, target_speed);
    }
    PRINT("slow to 1 kph");
    speed_emulate(1);
    sleep_ms(1000);


    // now check change speed to different amount
    {
        const float target_speed = 10.0f;
        speed_emulate(target_speed);
        sleep_ms(500);
        speed::start();
        float curent_speed = speed::get_velocity_kph();
        PICO_TEST_CHECK_VERBAL(curent_speed > 0, "speed: %.2f", curent_speed);

        //reset
        speed::get_distance_total(true);
        speed::get_time_total(true);
        // allow to run
        sleep_ms(1000);

        //read data
        const float dist_m = speed::get_distance_total_m(true);
        const float time_s = speed::get_time_total_s(true);
        speed::stop();
        const float speed = speed_mps_to_kmph(dist_m/time_s);
        PICO_TEST_CHECK_VERBAL(std::abs(speed - target_speed) < 0.5, "speed:%f target:%f", speed, target_speed);
    }

}


TEST_CASE_FUNCTION(tc_speeed)
{
    TEST_CASE_SUB_FUNCTION(sim_avg_test);
}

int main()
{
    traces_init();
    // TRACES_ON(TRACE_SPEED, 1); // speed update callback
    return tc_speeed().asserts_failed;
}