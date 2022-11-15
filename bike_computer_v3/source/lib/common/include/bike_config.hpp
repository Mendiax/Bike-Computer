#ifndef BIKE_CONFIG_HPP
#define BIKE_CONFIG_HPP

#include <vector>
#include <stdint.h>
#include <string>
#include <sstream>
#include <unordered_map>

#include "traces.h"

struct Gear_S
{
    uint8_t front;
    uint8_t rear;

    bool operator==(const Gear_S &other) const
    {
        return (front == other.front && rear == other.rear);
    }
    struct HashFunction
    {
        size_t operator()(const Gear_S& pos) const
        {
            uint16_t combined = (uint16_t)pos.front << 8 | (uint16_t)pos.rear;

            return std::hash<uint16_t>()(combined);
        }
    };
};

struct Bike_Config_S
{
    std::vector<uint8_t> gear_front; // chaing rings sizes for front
    std::vector<uint8_t> gear_rear; // chaing rings sizes for rear
    double wheel_size; // circumstance of wheel
    double min_gear_diff; // min difference of ratio betweens gears
    // std::unordered_map<Gear_S, float, Gear_S::HashFunction> gear_ratios;

    constexpr static inline bool is_gear_null(const Gear_S& gear)
    {
        return (gear.rear == 0 || gear.front == 0);
    }

    Gear_S get_next_gear(Gear_S gear) const;
    Gear_S get_prev_gear(Gear_S gear) const;
    float get_gear_ratio(Gear_S gear) const;

    std::string name;

    inline const char* get_file_name()
    {
        const std::string file_name = name + ".cfg";
        return file_name.c_str();
    }
    /**
     * @brief write current config to file
     *
     * File looks like this:
     * GF:{32} \\ gears on the front
     * GR:{51,45,39,33,28,24,21,18,15,13,11} \\ gears on the rear
     * WS:1.2 \\ wheel size in meters
     *
     *
     * @return const char*
     */
    const char* to_string();

    void from_string(const char* str);

    inline uint8_t to_idx(const Gear_S &gear)
    {
        return (gear.front - 1) * this->gear_rear.size() + (gear.rear - 1);
    }

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
#define CADENCE_DATA_MIN 60
#define CADENCE_DATA_MAX 110

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
    void from_string(const std::string& str);
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
    TRACE_DEBUG(3, TRACE_BIKE_CONFIG,"added gear usage for gear %d with cadence id %d made it to %f\n", (int)gear, (int)cadence_int, gear_usage.usage[gear - 1][cadence_int]);
}

#endif
