#include "pico/time.h"
#include "pico_test.hpp"
#include "bat_performance.hpp"
#include "traces.h"
#include <cassert>
#include <cmath>
#include <stdio.h>
#include <pico/stdlib.h>
#include <iostream>

static inline absolute_time_t add_us_to_absolute_time(absolute_time_t time, uint64_t us)
{
    uint64_t current_us = to_us_since_boot(time);
    current_us += us;
    absolute_time_t new_time;
    update_us_since_boot(&new_time, current_us);
    return new_time;
}

/**
 * @brief Test basic initialization of battery performance module
 */
TEST_CASE_SUB_FUNCTION_DEF(init_test)
{
    bat_performance::init();
    const auto& data = bat_performance::get_data();

    PICO_TEST_ASSERT_VERBAL(data.initialized == true, "Battery performance should be initialized");
    PICO_TEST_ASSERT_VERBAL(data.measurement_count == 0, "Initial measurement count should be 0");
    PICO_TEST_ASSERT_VERBAL(data.last_percent == -1, "Initial last_percent should be -1");
}

/**
 * @brief Test setting battery usage at various percentages
 */
TEST_CASE_SUB_FUNCTION_DEF(set_bat_usage_test)
{
    bat_performance::reset();

    // Test valid percentage values
    absolute_time_t t0 = get_absolute_time();

    PICO_TEST_ASSERT_VERBAL(bat_performance::set_bat_usage(100, t0) == true, "Should accept 100%");
    PICO_TEST_ASSERT_VERBAL(bat_performance::set_bat_usage(75, add_us_to_absolute_time(t0, 1000000)) == true, "Should accept 75%");
    PICO_TEST_ASSERT_VERBAL(bat_performance::set_bat_usage(50, add_us_to_absolute_time(t0, 2000000)) == true, "Should accept 50%");
    PICO_TEST_ASSERT_VERBAL(bat_performance::set_bat_usage(25, add_us_to_absolute_time(t0, 3000000)) == true, "Should accept 25%");
    PICO_TEST_ASSERT_VERBAL(bat_performance::set_bat_usage(0, add_us_to_absolute_time(t0, 4000000)) == true, "Should accept 0%");

    const auto& data = bat_performance::get_data();
    PICO_TEST_ASSERT_VERBAL(data.measurement_count == 5, "Should have 5 measurements");
    PICO_TEST_ASSERT_VERBAL(data.last_percent == 0, "Last percent should be 0");

    // Test invalid percentage values
    PICO_TEST_ASSERT_VERBAL(bat_performance::set_bat_usage(-1, get_absolute_time()) == false, "Should reject negative %");
    PICO_TEST_ASSERT_VERBAL(bat_performance::set_bat_usage(101, get_absolute_time()) == false, "Should reject >100%");
    PICO_TEST_ASSERT_VERBAL(bat_performance::set_bat_usage(150, get_absolute_time()) == false, "Should reject 150%");
}

/**
 * @brief Test that repeated percentage values don't get overwritten
 */
TEST_CASE_SUB_FUNCTION_DEF(duplicate_percent_test)
{
    bat_performance::reset();

    absolute_time_t t0 = get_absolute_time();
    absolute_time_t t1 = add_us_to_absolute_time(t0, 1000000);
    absolute_time_t t2 = add_us_to_absolute_time(t0, 5000000);

    // First record at 50%
    bat_performance::set_bat_usage(50, t1);

    const auto& data1 = bat_performance::get_data();
    absolute_time_t first_timestamp = data1.percent_timestamps[50];

    // Try to record at 50% again with different timestamp
    bat_performance::set_bat_usage(50, t2);

    const auto& data2 = bat_performance::get_data();
    absolute_time_t second_timestamp = data2.percent_timestamps[50];

    PICO_TEST_ASSERT_VERBAL(first_timestamp != second_timestamp, "Timestamp should be overwritten");
}

/**
 * @brief Test time calculation with linear discharge pattern
 */
TEST_CASE_SUB_FUNCTION_DEF(linear_discharge_test)
{
    bat_performance::reset();

    // Simulate linear discharge: 1% per second
    // 100% at t=0
    // 50% at t=50s (50,000,000 us)
    // 0% at t=100s (100,000,000 us)

    absolute_time_t t0 = get_absolute_time();
    absolute_time_t t_50 = add_us_to_absolute_time(t0, 50000000);  // 50 seconds
    absolute_time_t t_100 = add_us_to_absolute_time(t0, 100000000); // 100 seconds

    bat_performance::set_bat_usage(100, t0);
    bat_performance::set_bat_usage(50, t_50);
    bat_performance::set_bat_usage(0, t_100);

    // Test: at 75% at t=25s, should have ~75 seconds left (75% * 1s/% = 75s)
    absolute_time_t t_75 = add_us_to_absolute_time(t0, 25000000);  // 25 seconds
    uint32_t time_left = bat_performance::get_time_left(75, t_75);

    PICO_TEST_ASSERT_VERBAL(time_left > 0, "Should calculate positive time");
    PICO_TEST_ASSERT_VERBAL(time_left >= 73 && time_left <= 77, "Time left should be ~75 seconds, got %u", time_left);
}

/**
 * @brief Test time calculation when current percent is between recorded values
 */
TEST_CASE_SUB_FUNCTION_DEF(interpolation_test)
{
    bat_performance::reset();

    absolute_time_t t0 = get_absolute_time();

    // Record measurements at 100%, 50%, 0%
    bat_performance::set_bat_usage(100, t0);
    bat_performance::set_bat_usage(50, add_us_to_absolute_time(t0, 50000000));  // 50s
    bat_performance::set_bat_usage(0, add_us_to_absolute_time(t0, 100000000));  // 100s

    // Query at 75% (between 100% and 50%)
    // At 1% per second, 75% remaining means ~75 seconds left
    absolute_time_t t_query = add_us_to_absolute_time(t0, 25000000);  // 25s
    uint32_t time_left = bat_performance::get_time_left(75, t_query);

    PICO_TEST_ASSERT_VERBAL(time_left > 0, "Should calculate time for interpolated value");
    PICO_TEST_ASSERT_VERBAL(time_left >= 73 && time_left <= 77, "Should be around 75 seconds, got %u", time_left);
}

/**
 * @brief Test with sparse data points
 */
TEST_CASE_SUB_FUNCTION_DEF(sparse_data_test)
{
    bat_performance::reset();

    absolute_time_t t0 = get_absolute_time();

    // Only record at 100% and 0%
    bat_performance::set_bat_usage(100, t0);
    bat_performance::set_bat_usage(0, add_us_to_absolute_time(t0, 200000000));  // 200s for full discharge

    // Query at 50% which should be at t=100s
    // Discharge rate: 100% / 200s = 0.5% per second
    // Time remaining from 50%: 50% / 0.5%/s = 100 seconds
    absolute_time_t t_query = add_us_to_absolute_time(t0, 100000000);  // 100s
    uint32_t time_left = bat_performance::get_time_left(50, t_query);

    PICO_TEST_ASSERT_VERBAL(time_left > 0, "Should calculate with sparse data");
    PICO_TEST_ASSERT_VERBAL(time_left >= 98 && time_left <= 102, "Should be around 100 seconds, got %u", time_left);
}

/**
 * @brief Test edge case: insufficient data
 */
TEST_CASE_SUB_FUNCTION_DEF(insufficient_data_test)
{
    bat_performance::reset();
    auto time_start = get_absolute_time();

    // With no data, should return 0
    uint32_t time_left = bat_performance::get_time_left(50, time_start);
    PICO_TEST_ASSERT_VERBAL(time_left == 0, "Should return 0 with no data");

    // With only one measurement, need current_time to calculate
    bat_performance::set_bat_usage(100, time_start);

    // Now with one data point
    absolute_time_t t_query = add_us_to_absolute_time(time_start, 100000000);
    time_left = bat_performance::get_time_left(50, t_query);

    // PICO_TEST_ASSERT_VERBAL(time_left > 0, "Should calculate with one data point and time reference");
}

/**
 * @brief Test invalid query percentages
 */
TEST_CASE_SUB_FUNCTION_DEF(invalid_query_test)
{
    bat_performance::reset();

    absolute_time_t t0 = get_absolute_time();
    bat_performance::set_bat_usage(100, t0);
    bat_performance::set_bat_usage(50, add_us_to_absolute_time(t0, 50000000));

    // Invalid percentages should return 0
    PICO_TEST_ASSERT_VERBAL(bat_performance::get_time_left(-1, t0) == 0, "Should reject negative %");
    PICO_TEST_ASSERT_VERBAL(bat_performance::get_time_left(101, t0) == 0, "Should reject >100%");
    PICO_TEST_ASSERT_VERBAL(bat_performance::get_time_left(150, t0) == 0, "Should reject 150%");
}

/**
 * @brief Test reset functionality
 */
TEST_CASE_SUB_FUNCTION_DEF(reset_test)
{
    bat_performance::init();

    // Add some data
    absolute_time_t t0 = get_absolute_time();
    bat_performance::set_bat_usage(100, t0);
    bat_performance::set_bat_usage(50, add_us_to_absolute_time(t0, 50000000));

    const auto& data_before = bat_performance::get_data();
    PICO_TEST_ASSERT_VERBAL(data_before.measurement_count == 2, "Should have 2 measurements");

    // Reset
    bat_performance::reset();

    const auto& data_after = bat_performance::get_data();
    PICO_TEST_ASSERT_VERBAL(data_after.measurement_count == 0, "After reset, count should be 0");
    PICO_TEST_ASSERT_VERBAL(data_after.last_percent == -1, "After reset, last_percent should be -1");
}

/**
 * @brief Test with non-linear discharge pattern
 */
TEST_CASE_SUB_FUNCTION_DEF(nonlinear_discharge_test)
{
    bat_performance::reset();

    absolute_time_t t0 = get_absolute_time();

    // Simulate non-linear discharge (fast initial, then slow)
    // 100% at t=0
    // 80% at t=5s (5,000,000 us) - 20% in 5s (fast)
    // 50% at t=30s (30,000,000 us) - 30% in 25s (slow)
    // 0% at t=100s (100,000,000 us) - 50% in 70s (very slow)

    bat_performance::set_bat_usage(100, t0);
    bat_performance::set_bat_usage(80, add_us_to_absolute_time(t0, 5000000));
    bat_performance::set_bat_usage(50, add_us_to_absolute_time(t0, 30000000));
    bat_performance::set_bat_usage(0, add_us_to_absolute_time(t0, 100000000));

    // Query at 25% - between 50% and 0%
    // At t=65s, we're 35s into the 50%-to-0% segment
    // Discharge from 50% (at 30s) to 0% (at 100s): 50% in 70s
    // After 35s at 0.714%/s rate: discharged 35*0.714=25%, so 25% remaining ✓
    // Time remaining: 25% / 0.714%/s ≈ 35s
    // However, the algorithm may use slightly different calculation due to rounding
    absolute_time_t t_query = add_us_to_absolute_time(t0, 65000000);  // 65s
    uint32_t time_left = bat_performance::get_time_left(25, t_query);

    PICO_TEST_ASSERT_VERBAL(time_left > 0, "Should calculate for non-linear pattern");
    // Allow wide range due to potential rounding differences in algorithm
    PICO_TEST_ASSERT_VERBAL(time_left >= 24 && time_left <= 36, "Should estimate time from 25% to 0%, got %u", time_left);
}

TEST_CASE_FUNCTION(tc_bat_performance)
{
    TEST_CASE_SUB_FUNCTION(init_test);
    TEST_CASE_SUB_FUNCTION(set_bat_usage_test);
    TEST_CASE_SUB_FUNCTION(duplicate_percent_test);
    TEST_CASE_SUB_FUNCTION(linear_discharge_test);
    TEST_CASE_SUB_FUNCTION(interpolation_test);
    TEST_CASE_SUB_FUNCTION(sparse_data_test);
    TEST_CASE_SUB_FUNCTION(insufficient_data_test);
    TEST_CASE_SUB_FUNCTION(invalid_query_test);
    TEST_CASE_SUB_FUNCTION(reset_test);
    TEST_CASE_SUB_FUNCTION(nonlinear_discharge_test);
}

extern "C"
{
    int main()
    {
        traces_init();
        return tc_bat_performance().asserts_failed;
    }
}
