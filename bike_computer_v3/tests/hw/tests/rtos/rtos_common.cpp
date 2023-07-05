#include "FreeRTOS.h"
#include "semphr.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico_test.hpp"


// TEST_CASE_FUNCTION(rtos_common)
// {

// }

static int tasks_left = 0;
static SemaphoreHandle_t xMutex;

void finish_task()
{
    xSemaphoreTake(xMutex, portMAX_DELAY);
    tasks_left--;
    xSemaphoreGive(xMutex);
}



static void stop_all(void *pvParameters)
{
    (void)pvParameters;
    const int delay = *((int*)pvParameters);
    vTaskDelay(pdMS_TO_TICKS(delay));
    printf("vTaskEndScheduler()\n");

    while (1) {
        auto left = 0;
        xSemaphoreTake(xMutex, portMAX_DELAY);
        left = tasks_left;
        xSemaphoreGive(xMutex);
        if(left > 0)
            vTaskDelay(pdMS_TO_TICKS(100));
        else
            break;
    }

    // freeze
    vTaskSuspendAll();
    pico_test_end();
    while (1) {
        sleep_ms(500);
        printf("blink\n");
    }
}

void create_stop_all_task(int time_limit_ms, int task_counter)
{
    tasks_left = task_counter;
    xMutex = xSemaphoreCreateMutex();
    static int time_limit = time_limit_ms;
    xTaskCreate(stop_all, "STOP_Task", 256, &time_limit, tskIDLE_PRIORITY, NULL);
}
