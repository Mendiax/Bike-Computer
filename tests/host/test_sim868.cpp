// #include "traces.h"
#include "pico/time.h"
#include "pico_test.hpp"
#include "sim868/gps.hpp"
#include "traces.h"
#include <cassert>
#include <cmath>
#include <pico/stdlib.h>
#include "sim868/interface.hpp"

#include "mock_sim868.hpp"


TEST_CASE_SUB_FUNCTION_DEF(uart_test)
{
    Mock_SIM868 mock_sim868;
    mock_sim868.run();
    sim868::boot();

    while (sim868::gps::fetch_data() == false) {
        sleep_ms(100);
    }

    float speed;
    sim868::gps::get_speed(speed);
    TimeS time{};
    sim868::gps::get_date(time);
    PICO_TEST_CHECK(time.year >= 2023);
    PICO_TEST_CHECK_EQ(speed, 10);
}


TEST_CASE_FUNCTION(tc_sim868)
{
    TEST_CASE_SUB_FUNCTION(uart_test);
}

int main()
{
    traces_init();
    // TRACES_ON(TRACE_SPEED, 1); // speed update callback
    return tc_sim868().asserts_failed;
}