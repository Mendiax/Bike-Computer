#include "bat_performance.hpp"
#include <algorithm>
#include <cmath>

// Static instance of battery performance data
static bat_performance::BatteryPerformanceData g_bat_performance_data;

namespace bat_performance
{
    void init()
    {
        g_bat_performance_data.percent_timestamps.fill(0);
        g_bat_performance_data.measurement_count = 0;
        g_bat_performance_data.last_percent = -1;
        g_bat_performance_data.initialized = true;
    }

    bool set_bat_usage(int percent, absolute_time_t timestamp)
    {
        // Validate percentage is in valid range
        if (percent < 0 || percent > 100)
        {
            return false;
        }

        // Record timestamp for this percentage if not already recorded
        g_bat_performance_data.percent_timestamps[percent] = timestamp;

        // reset the timestamps for lower percentages if we are recording a higher percentage (battery got charged)
        for (int i = BAT_PERFORMANCE_LEVELS - 1; i > percent; i--)
        {
            g_bat_performance_data.percent_timestamps[i] = 0;
        }

        g_bat_performance_data.measurement_count++;
        g_bat_performance_data.last_percent = percent;

        return true;
    }

    float get_time_left(int current_percent, absolute_time_t current_time)
    {
        if (!set_bat_usage(current_percent, current_time)) {
            return 0; // Invalid percentage, cannot calculate time left
        }

        // calculate average time per percentage drop based on recorded data
        // get first and last recorded percentage levels that is not 0 and calculate the average time per percentage drop
        int first_timestamp = -1;
        int last_timestamp = -1;
        for (int i = 0; i < BAT_PERFORMANCE_LEVELS; i++)
        {
            if (g_bat_performance_data.percent_timestamps[i] != 0)
            {
                if (first_timestamp == -1)
                {
                    first_timestamp = i;
                }
                last_timestamp = i;
            }
        }

        if (first_timestamp == -1 || last_timestamp == -1)
        {
            return 0;
        }
        const auto total_time = absolute_time_diff_us(g_bat_performance_data.percent_timestamps[last_timestamp], g_bat_performance_data.percent_timestamps[first_timestamp]);
        const uint64_t count = last_timestamp - first_timestamp;

        if (count == 0)
        {
            return 0;
        }
        float avg_time_per_percent = static_cast<float>(total_time) / static_cast<float>(count);
        return (avg_time_per_percent *  static_cast<float>(current_percent)) /
               1000000.f; // convert microseconds to seconds
    }

    const BatteryPerformanceData& get_data()
    {
        return g_bat_performance_data;
    }

    void reset()
    {
        init();
    }
}
