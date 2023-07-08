#include "pico/time.h"
#include "pico_test.hpp"
#include "pico/stdlib.h"
#include <cstdint>
#include <stdio.h>
#include <inttypes.h>


#include "traces.h"


// SD
#include "sd_file.h"
#include "ff.h"
#include "session.hpp"
#include "sd_common.h"

#define TEST_FOLDER "__test__"

/**
writes a thp of
*/
double get_write_thp(Sd_File& sd, size_t size_b)
{

    char* test_string = new char[size_b];
    memset(test_string, 'a', size_b);
    test_string[size_b - 1] = '\0';

    const int no_times = 100;
    auto time_start = get_absolute_time();
    for(int i = 0; i < no_times; i++)
    {
        sd.append(test_string);
    }
    auto time_end = get_absolute_time();
    auto time_us = absolute_time_diff_us(time_start, time_end);

    time_us /= no_times;

    delete[] test_string;
    double time_s = (double)time_us / 1000000.0;
    // TODO different calc for big/small numbers
    return (double)size_b/time_s;
}


// static void TEST_CASE_SUB_FUNCTION_DEF_ARGS(check_write, Sd_File& sd, size_t size_b, double expected_thp)
// {
//     double thp = get_write_thp(sd, size_b);
//     PICO_TEST_CHECK_VERBAL(thp > expected_thp, "size: %5zu, expected: %10.2lf, actual: %10.2lf", size_b, expected_thp, thp);
// }


TEST_CASE_SUB_FUNCTION_DEF(sd_drive_thp_test)
{
    Sd_File test_file("__test__.txt");
    test_file.clear();


    for(int i = 1; i < 12; i++)
    {
        const size_t size = 1 << i;
        // TEST_CASE_SUB_FUNCTION_ARGS(check_write, test_file, size , 1.0);
        const double thp = get_write_thp(test_file, size);
        const double expected_thp = 1.0;
        PICO_TEST_CHECK_VERBAL(thp > expected_thp, "size: %5zu, expected: %10.2lf, actual: %10.2lf", size, expected_thp, thp);
    }
    // TEST_CASE_SUB_FUNCTION_ARGS(check_write, test_file, 8, 1.0);
    // TEST_CASE_SUB_FUNCTION_ARGS(check_write, test_file, 16, 1.0);
    // TEST_CASE_SUB_FUNCTION_ARGS(check_write, test_file, 32, 1.0);
    // TEST_CASE_SUB_FUNCTION_ARGS(check_write, test_file, 4, 1.0);
    // TEST_CASE_SUB_FUNCTION_ARGS(check_write, test_file, 4, 1.0);



    // {
    //     size_t size_b = 4;
    //     double expected_thp = 1.0;
    //     double thp = get_write_thp(test_file, size_b);
    //     PICO_TEST_CHECK_VERBAL(thp > expected_thp, "expected: %lf, actual: %lf", expected_thp, thp);
    // }

    test_file.remove();
}


TEST_CASE_FUNCTION(tc_sd_thp)
{
    dir::del(TEST_FOLDER);
    f_mkdir(TEST_FOLDER);

    int64_t time_sleep_us = 10000000;
    auto time_start = get_absolute_time();
    sleep_ms(time_sleep_us/1000); // 1s
    auto time_end = get_absolute_time();
    auto time_us = absolute_time_diff_us(time_start, time_end);
    // expected time_us -> 1 000 0000
    time_us -= time_sleep_us;
    time_us = time_us < 0 ? -time_us : time_us;
    PICO_TEST_ASSERT_VERBAL(time_us <= 1000, "dt = %" PRId64 "us", time_us); // less than 1ms accuracy


    TEST_CASE_SUB_FUNCTION(sd_drive_thp_test);
    dir::del(TEST_FOLDER);

    PICO_TEST_ASSERT(1 == 1);
}
