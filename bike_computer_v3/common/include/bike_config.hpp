#ifndef BIKE_CONFIG_HPP
#define BIKE_CONFIG_HPP

#include <vector>
#include <stdint.h>
#include <string>

struct Gear_S
{
    uint8_t front;
    uint8_t rear;
};

struct Bike_Config_S
{
    std::vector<uint8_t> gear_front; // chaing rings sizes for front
    std::vector<uint8_t> gear_rear; // chaing rings sizes for rear
    double wheel_size; // circumstance of wheel

    uint8_t to_idx(const Gear_S &gear);

    Gear_S get_current_gear(float ratio);
    float get_gear_ratio(Gear_S gear);
};

/**
 * @brief sets gears in Bike_Config_S
 *
 */
#define BIKE_CONFIG_SET_GEARS(bike_config, arr_gears_front,  arr_gears_rear) \
    do{ \
        { \
            bike_config.gear_front.clear(); \
            for(size_t i = 0; i < sizeof(arr_gears_front) / sizeof(arr_gears_front[0]); i++) \
            { \
                bike_config.gear_front.emplace_back(arr_gears_front[i]); \
            } \
        } \
        { \
            bike_config.gear_rear.clear(); \
            for(size_t i = 0; i < sizeof(arr_gears_rear) / sizeof(arr_gears_rear[0]); i++) \
            { \
                bike_config.gear_rear.emplace_back(arr_gears_rear[i]); \
            } \
        } \
    } while(0)

#define MAX_NO_OF_GEARS 11
#define CADENCE_DATA_DELTA 5
#define CADENCE_DATA_MIN 50
#define CADENCE_DATA_MAX 120

#define CADENCE_DATA_LEN ((CADENCE_DATA_MAX - CADENCE_DATA_MIN) / CADENCE_DATA_DELTA + 2)

/**
 * @brief [gear] -> [<50, 50, 55, >60, ..., >=120]
           usage in seconds per cadence
 *
 */
struct Gear_Usage
{
    float usage[MAX_NO_OF_GEARS][CADENCE_DATA_LEN];
    /**
     * @brief returns gear usage is string
     * [no_gears, min, max, delta]
     *
     * @return std::string
     */
    std::string to_string();
};

/**
 * @brief add time to gear
 *
 * @param gear_usage
 * @param gear
 * @param cadence
 * @param time in seconds
 */
static inline void add_gear_usage(Gear_Usage &gear_usage, uint8_t gear, float cadence, float time)
{
    uint_fast16_t cadence_int = cadence + 0.5f;
    if (cadence_int < CADENCE_DATA_MIN)
    {
        cadence_int = 0;
    }
    else if (cadence_int >= CADENCE_DATA_MAX)
    {
        cadence_int = CADENCE_DATA_LEN - 1;
    }
    else
    {
        cadence_int -= CADENCE_DATA_MIN;
        // cadence_int in [0 ... 69]
        cadence_int /= CADENCE_DATA_DELTA;
        cadence_int += 1;
    }
    gear_usage.usage[gear - 1][cadence_int] += time;
}

#endif