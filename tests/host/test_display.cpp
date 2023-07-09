#include <stdio.h>
#include <pico/stdlib.h>
#include "bc_test.h"
#include "display/display_config.hpp"
#include "display/driver.hpp"
#include "pico/time.h"



int test_display_basic()
{
    display::init();
    display::clear();
    display::draw_line(0, 0, DISPLAY_WIDTH -1, DISPLAY_HEIGHT-1);
    display::display();
    sleep_ms(3000);
    return 0;
}

int main()
{
    BC_TEST(test_display_basic);
    BC_TEST_END();
    sleep_ms(1000);
    return 0;
}
