#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"

#include "tests/test_cases.h"
#include "bc_test.h"
#include "pico_test.hpp"
#include "traces.h"

#include "interrupts/interrupts.hpp"
#include "speedometer/speedometer.hpp"
#include "cadence/cadence.hpp"


#define LED_PIN 25


int test_basic()
{
    BC_CHECK_EQ(1,1);
    return 0;
}

int main()
{
    bi_decl(bi_program_description("First Blink"));
    bi_decl(bi_1pin_with_name(LED_PIN, "On-board LED"));

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 1);
    printf("main()\n");

    stdio_init_all();
    traces_init();

    interruptSetupCore0();
    interruptSetupCore1();

    pico_test_start();

    printf("second test file\n");

    pico_test_end();
    while (true) {
        gpio_put(LED_PIN, 0);
        sleep_ms(500);
        gpio_put(LED_PIN, 1);
        sleep_ms(500);
        printf("blink\n");
    }
}

