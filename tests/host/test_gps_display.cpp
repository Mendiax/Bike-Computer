#include "pico_test.hpp"
#include <pico/stdlib.h>
#include "traces.h"

#include "display_actor.hpp"

// #include <stdio.h>
// #include "display/display_config.hpp"
// #include "display/driver.hpp"
// #include "pico/time.h"

// #include "display_functions_mock.hpp"


TEST_CASE_SUB_FUNCTION_DEF(display_points)
{
    auto& display_actor = Display_Actor::get_instance();

}


TEST_CASE_FUNCTION(tc_gps_display)
{
    TEST_CASE_SUB_FUNCTION(display_points);
}

int main()
{
    traces_init();
    return tc_gps_display().asserts_failed;
}