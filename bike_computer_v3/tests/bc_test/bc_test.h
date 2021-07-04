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
#include <iostream>


#define ERROR 1

#define BC_CHECK_VERBAL(statement, fmt, ...) if(!(statement)){fprintf(stderr, "[ERROR] line:%d " #statement " failed " fmt "\n",  __LINE__, ##__VA_ARGS__); return ERROR;}
#define BC_CHECK(statement) if(!(statement)){fprintf(stderr, "[ERROR] line:%d " #statement " failed \n", __LINE__); return ERROR;}
#define BC_CHECK_EQ(x,y) if(x != y){std::cerr << __LINE__  << ":" #x ":" << x << "!=" #y ":" << y << std::endl; return ERROR;}

extern int bc_testError;

#define BC_TEST_START()                \
    do                                 \
    {                                  \
        stdio_init_all();              \
        while (!stdio_usb_connected()) \
        {                              \
            sleep_ms(100);             \
        }                              \
    } while (0)

#define BC_TEST(funcName)                                                                        \
    do                                                                                           \
    {                                                                                            \
        printf("[TEST] " #funcName " started\n");                                                \
        absolute_time_t timeStart = get_absolute_time();                                         \
        int ret = funcName();                                                                    \
        absolute_time_t timeEnd = get_absolute_time();                                           \
        long long int timeTaken = absolute_time_diff_us(timeStart, timeEnd);                     \
        if (ret != 0)                                                                            \
        {                                                                                        \
            printf("\x1b[1;31m[TEST] " #funcName " FAILED!   Took %lld us\n\x1b[0m", timeTaken); \
            bc_testError++;                                                                      \
        }                                                                                        \
        else                                                                                     \
        {                                                                                        \
            printf("\x1b[1;32m[TEST] " #funcName " PASSED!   Took %lld us\n\x1b[0m", timeTaken); \
        }                                                                                        \
    } while (0)

/*#define BC_CHECK(boolean)                                              \
    do                                                                 \
    {                                                                  \
        if (!(boolean))                                                \
        {                                                              \
            printf("\x1b[1;31m[TEST] " #boolean " failed\n\x1b[0m\n"); \
            return 1;                                                  \
        }                                                              \
    } while (0)
*/
#define BC_TEST_END()                                     \
    do                                                    \
    {                                                     \
        printf("[TEST] finished\n");                      \
        if (bc_testError == 0)                            \
            printf("\x1b[1;32m[TEST] PASSED!\x1b[0m\n");  \
        else                                              \
            printf("\x1b[1;31m[TEST] failed\x1b[0m\n"); \
    } while (0)

#endif