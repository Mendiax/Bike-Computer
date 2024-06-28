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

struct Bike_Config
{
    struct Gear_Ratio_Range{
        float min;
        float max;
    };
    std::vector<Gear_Ratio_Range> gear_ranges;
    std::vector<uint8_t> gear_front; // chaing rings sizes for front
    std::vector<uint8_t> gear_rear; // chaing rings sizes for rear

    double wheel_size; // circumstance of wheel
    double min_gear_diff; // min difference of ratio betweens gears
    // std::unordered_map<Gear_S, float, Gear_S::HashFunction> gear_ratios;
    int hour_offset;
    uint16_t cadence_min;
    uint16_t cadence_max;
    uint8_t no_magnets_cadence;
    uint8_t no_magnets_speed;



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
     * WS:2.2 \\ wheel size in meters
     *
     *
     * @return const char*
     */
    const char* to_string();

    bool from_string(const char* str);

    inline uint8_t to_idx(const Gear_S &gear)
    {
        return (gear.front - 1) * this->gear_rear.size() + (gear.rear - 1);
    }

    Gear_S get_current_gear(float ratio);
    float get_gear_ratio(Gear_S gear);
};

/**
 * @brief sets gears in Bike_Config
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


#endif
