#ifndef BAT_PERFORMANCE_HPP
#define BAT_PERFORMANCE_HPP

// #-------------------------------#
// |           includes            |
// #-------------------------------#

#include <stdint.h>
#include <array>
#include "pico/time.h"  // For absolute_time_t

// #-------------------------------#
// |            macros             |
// #-------------------------------#

/// @brief Number of battery percentage levels tracked (0-100%)
#define BAT_PERFORMANCE_LEVELS 101

// #-------------------------------#
// | global types declarations     |
// #-------------------------------#

/**
 * @brief Module for tracking battery performance and calculating discharge time
 */
namespace bat_performance
{
    /**
     * @brief Structure to hold battery performance data
     */
    struct BatteryPerformanceData
    {
        /// Array storing timestamp (microseconds from boot) for each percentage level (0-100%)
        /// Index represents battery percentage, value is absolute time when that percentage was reached
        /// A value of 0 means that percentage has not been reached yet
        std::array<absolute_time_t, BAT_PERFORMANCE_LEVELS> percent_timestamps;

        /// Total number of measurements recorded
        uint32_t measurement_count;

        /// Last recorded battery percentage
        int last_percent;

        /// Initialization flag
        bool initialized;
    };

    /**
     * @brief Initializes the battery performance tracker
     */
    void init();

    /**
     * @brief Records battery usage at a specific percentage and timestamp
     *
     * @param percent Battery percentage (0-100)
     * @param timestamp Absolute time from boot in microseconds when this percentage was measured
     *
     * @return true if the value was recorded successfully, false if percent is out of range
     */
    bool set_bat_usage(int percent, absolute_time_t timestamp);

    /**
     * @brief Calculates the estimated time remaining until battery discharge
     *
     * Extrapolates from the recorded battery usage pattern to estimate how many seconds
     * until the battery reaches 0%.
     *
     * @param current_percent Current battery percentage (0-100)
     * @param current_time Current absolute time in microseconds from boot
     *
     * @return Estimated seconds until battery discharge (0 if unable to calculate)
     */
    float get_time_left(int current_percent, absolute_time_t current_time);

    /**
     * @brief Gets the current battery performance data structure
     *
     * @return Const reference to the battery performance data
     */
    const BatteryPerformanceData& get_data();

    /**
     * @brief Resets all battery performance data
     */
    void reset();
};

#endif // BAT_PERFORMANCE_HPP
