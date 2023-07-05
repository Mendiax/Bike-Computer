#include "pico_test.hpp"
#include "pico/stdlib.h"
#include <cstdint>
#include <stdio.h>

#include "signals.hpp"
#include "traces.h"
#include "actors.hpp"
#include "rtos/rtos_common.hpp"

#include <FreeRTOS.h>
#include <task.h>

static int counter = 0;

static void led_task(void *pvParameters)
{
    (void)pvParameters;
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    while (true) {
        counter++;
        gpio_put(LED_PIN, 1);
        vTaskDelay(pdMS_TO_TICKS(100));
        gpio_put(LED_PIN, 0);
        vTaskDelay(pdMS_TO_TICKS(100));
        printf("led tesk loop\n");
    }
}



TEST_CASE_FUNCTION(tc_rtos_basic)
{
    create_stop_all_task(1000);
    xTaskCreate(led_task, "LED_Task", 256, NULL, 1, NULL);
    printf("vTaskStartScheduler()\n");
    vTaskStartScheduler();

    PICO_TEST_ASSERT(counter > 0);
}
