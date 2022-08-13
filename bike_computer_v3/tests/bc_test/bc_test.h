#ifndef BC_TEST_H
#define BC_TEST_H
/**
 * @file test_framework.h
 * @author your name (you@domain.com)
 * @brief framework for writeing tests for raspberry pi pico
 * @version 0.1
 * @date 2022-07-18
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <stdio.h>
#include <pico/time.h>
#include <pico/stdio.h>
#include <pico/stdlib.h>

#define ERROR 1

#define BC_TEST_START()                \
    do                                 \
    {                                  \
        stdio_init_all();              \
        while (!stdio_usb_connected()) \
        {                              \
            sleep_ms(100);             \
        }                              \
    } while (0)

#define BC_TEST(funcName)                                                                \
    do                                                                                   \
    {                                                                                    \
        printf("[TEST] " #funcName " started\n");                                          \
        absolute_time_t timeStart = get_absolute_time();                                 \
        int ret = funcName();                                                            \
        absolute_time_t timeEnd = get_absolute_time();                                   \
        long long int timeTaken = absolute_time_diff_us(timeStart, timeEnd);             \
        if (ret != 0)                                                                    \
        {                                                                                \
            printf("\x1b[1;31m[TEST] " #funcName " FAILED!   Took %lld us\n\x1b[0m", timeTaken); \
        }                                                                                \
        else                                                                             \
        {                                                                                \
            printf("\x1b[1;32m[TEST] " #funcName " PASSED!   Took %lld us\n\x1b[0m", timeTaken); \
        }                                                                                \
    } while (0)

#define BC_TEST_END()                \
    do                               \
    {                                \
        printf("[TEST] finished\n"); \
        for (;;)                     \
        {                            \
            sleep_ms(100);           \
        }                            \
    } while (0)

#endif