#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"

#include "pico/time.h"
#include "tests/test_cases.h"
#include "bc_test.h"
#include "pico_test.hpp"
#include "traces.h"

#include <FreeRTOS.h>
#include <task.h>


void led_task(void *pvParameters)
{
    (void)pvParameters;
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    while (true) {
        gpio_put(LED_PIN, 1);
        vTaskDelay(pdMS_TO_TICKS(100));
        gpio_put(LED_PIN, 0);
        vTaskDelay(pdMS_TO_TICKS(100));
        printf("led tesk loop\n");
    }
}

void stop_all(void *pvParameters)
{
    (void)pvParameters;
    const int delay = *((int*)pvParameters);
    vTaskDelay(pdMS_TO_TICKS(delay));
    printf("vTaskEndScheduler()\n");
    // vTaskEndScheduler();

    // freeze
    vTaskSuspendAll();
    pico_test_end();
    while (true) {
        sleep_ms(500);
        printf("blink\n");
    }
}

int main()
{
    bi_decl(bi_program_description("FreeRTOS test"));

    printf("main()\n");

    stdio_init_all();
    traces_init();

    pico_test_start();


    int time_limit = 5000;
    xTaskCreate(led_task, "LED_Task", 256, NULL, 1, NULL);
    xTaskCreate(stop_all, "STOP_Task", 256, &time_limit, tskIDLE_PRIORITY, NULL);
    printf("vTaskStartScheduler()\n");
    vTaskStartScheduler();




    pico_test_end();
    while (true) {
        sleep_ms(500);
        printf("blink\n");
    }
}

