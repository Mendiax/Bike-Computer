#ifndef __SPEEDMETER_H__
#define __SPEEDMETER_H__

// #-------------------------------#
// |           includes            |
// #-------------------------------#
// C/C++ includes
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "common_types.h"

// My includes

// #-------------------------------#
// |            macros             |
// #-------------------------------#

#define PIN_SPEED 16 ///< GPIO pin for speed measurement

#define MIN_SPEED 1.0 ///< Minimum speed in m/s

// #-------------------------------#
// | global types declarations     |
// #-------------------------------#

// No global types declarations

// #-------------------------------#
// | global variables declarations |
// #-------------------------------#

// No global variables declarations

// #-------------------------------#
// | global function declarations  |
// #-------------------------------#

/**
 * @brief Emulates a speed for testing purposes.
 * @param speed The speed to emulate in km/h (default: 20.0 km/h).
 */
void speed_emulate(float speed = 20.0);

/**
 * @brief Emulates a slowing down of speed for testing purposes.
 * @param speed The initial speed in km/h.
 * @param accel The deceleration rate in m/s^2.
 */
void speed_emulate_slowing(float speed, float accel);

namespace speed
{
    /**
     * @brief Sets up the speed measurement system.
     * @param wheel_diameter Wheel diameter in meters.
     * @param no_magnets Number of magnets on the wheel.
     */
    void setup(float wheel_diameter, uint8_t no_magnets);

    /**
     * @brief Gets the total time elapsed since the start or last reset.
     * @param reset If true, resets the total time after retrieval.
     * @return Total time in hours.
     */
    float get_time_total(bool reset = true);

    /**
     * @brief Gets the total distance traveled since the start or last reset.
     * @param reset If true, resets the total distance after retrieval.
     * @return Total distance in kilometers.
     */
    float get_distance_total(bool reset = true);

    /**
     * @brief Gets the total time elapsed since the start or last reset in seconds.
     * @param reset If true, resets the total time after retrieval.
     * @return Total time in seconds.
     */
    float get_time_total_s(bool reset = true);

    /**
     * @brief Gets the total distance traveled since the start or last reset in meters.
     * @param reset If true, resets the total distance after retrieval.
     * @return Total distance in meters.
     */
    float get_distance_total_m(bool reset = true);

    /**
     * @brief Calculates the current velocity in kilometers per hour (km/h).
     * @return Current velocity in km/h.
     */
    float get_velocity_kph();

    /**
     * @brief Gets the raw velocity without conversion in km/h.
     * @return Raw velocity in km/h.
     */
    float get_velocity_kph_raw();

    /**
     * @brief Converts velocity from km/h to revolutions per minute (RPM).
     * @param kph Velocity in kilometers per hour.
     * @return Velocity in RPM.
     */
    float kph_to_rpm(float kph);

    template<typename T>
    constexpr inline std::enable_if_t<std::is_floating_point_v<T>, T>
    kph_to_mps(T speed_kph) {
        constexpr T MPS_TO_KMPH = static_cast<T>(3.6);
        return speed_kph / MPS_TO_KMPH;
    }

    template<typename T>
    constexpr inline std::enable_if_t<std::is_floating_point_v<T>, T>
    mps_to_kph(T speed_mps) {
        constexpr T MPS_TO_KMPH = static_cast<T>(3.6);
        return speed_mps * MPS_TO_KMPH;
    }


    /**
     * @brief Gets the current distance traveled in meters.
     * @return Current distance in meters.
     */
    float get_distance_m();

    /**
     * @brief Starts the speed measurement.
     */
    void start();

    /**
     * @brief Stops the speed measurement.
     */
    void stop();

    /**
     * @brief Resets all speed-related variables to zero.
     */
    void reset();
}

// #-------------------------------#
// |  global function definitions  |
// #-------------------------------#

#endif // __SPEEDMETER_H__
