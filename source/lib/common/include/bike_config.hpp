#ifndef BIKE_CONFIG_HPP
#define BIKE_CONFIG_HPP

// #-------------------------------#
// |           includes            |
// #-------------------------------#

#include <vector>
#include <stdint.h>
#include <string>
#include <sstream>
#include <unordered_map>
#include "traces.h"

// #-------------------------------#
// |       global structures       |
// #-------------------------------#

/**
 * @brief Structure representing a gear with front and rear values.
 */
struct Gear_S
{
    uint8_t front; ///< Front gear value.
    uint8_t rear;  ///< Rear gear value.

    /**
     * @brief Equality operator for Gear_S.
     * @param other The other Gear_S to compare with.
     * @return True if both gears are equal, false otherwise.
     */
    bool operator==(const Gear_S &other) const
    {
        return (front == other.front && rear == other.rear);
    }

    /**
     * @brief Hash function for Gear_S.
     */
    struct HashFunction
    {
        /**
         * @brief Computes the hash for a Gear_S.
         * @param pos The Gear_S to compute the hash for.
         * @return The hash value.
         */
        size_t operator()(const Gear_S& pos) const
        {
            uint16_t combined = (uint16_t)pos.front << 8 | (uint16_t)pos.rear;
            return std::hash<uint16_t>()(combined);
        }
    };
};

/**
 * @brief Structure representing the bike configuration.
 */
struct Bike_Config
{
    struct Gear_Ratio_Range {
        float min; ///< Minimum gear ratio.
        float max; ///< Maximum gear ratio.
    };

    std::vector<Gear_Ratio_Range> gear_ranges; ///< Vector of gear ratio ranges.
    std::vector<uint8_t> gear_front; ///< Chainring sizes for the front gears.
    std::vector<uint8_t> gear_rear;  ///< Chainring sizes for the rear gears.

    double wheel_size; ///< Circumference of the wheel.
    double min_gear_diff; ///< Minimum difference of ratio between gears.

    int hour_offset; ///< Hour offset for the bike configuration.
    uint16_t cadence_min; ///< Minimum cadence.
    uint16_t cadence_max; ///< Maximum cadence.
    uint8_t no_magnets_cadence; ///< Number of magnets for cadence.
    uint8_t no_magnets_speed; ///< Number of magnets for speed.

    std::string name; ///< Name of the bike configuration.

    /**
     * @brief Checks if the gear is null.
     * @param gear The gear to check.
     * @return True if the gear is null, false otherwise.
     */
    constexpr static inline bool is_gear_null(const Gear_S& gear)
    {
        return (gear.rear == 0 || gear.front == 0);
    }

    Gear_S get_next_gear(Gear_S gear) const;
    Gear_S get_prev_gear(Gear_S gear) const;
    float get_gear_ratio(Gear_S gear) const;

    /**
     * @brief Gets the file name for the bike configuration.
     * @return The file name as a C-style string.
     */
    inline const char* get_file_name()
    {
        const std::string file_name = name + ".cfg";
        return file_name.c_str();
    }

    /**
     * @brief Converts the bike configuration to a string representation.
     * @return The string representation of the bike configuration.
     */
    const char* to_string();

    /**
     * @brief Parses the bike configuration from a string.
     * @param str The string to parse.
     * @return True if the parsing was successful, false otherwise.
     */
    bool from_string(const char* str);

    /**
     * @brief Converts a Gear_S to an index.
     * @param gear The gear to convert.
     * @return The index of the gear.
     */
    inline uint8_t to_idx(const Gear_S &gear)
    {
        return (gear.front - 1) * this->gear_rear.size() + (gear.rear - 1);
    }

    Gear_S get_current_gear(float ratio);
    float get_gear_ratio(Gear_S gear);
};

// #-------------------------------#
// |   global function declarations |
// #-------------------------------#

/**
 * @brief Sets the gears in Bike_Config.
 * @param bike_config The bike configuration to set the gears for.
 * @param arr_gears_front The array of front gears.
 * @param arr_gears_rear The array of rear gears.
 */
#define BIKE_CONFIG_SET_GEARS(bike_config, arr_gears_front, arr_gears_rear) \
    do { \
        { \
            bike_config.gear_front.clear(); \
            for (size_t i = 0; i < sizeof(arr_gears_front) / sizeof(arr_gears_front[0]); i++) \
            { \
                bike_config.gear_front.emplace_back(arr_gears_front[i]); \
            } \
        } \
        { \
            bike_config.gear_rear.clear(); \
            for (size_t i = 0; i < sizeof(arr_gears_rear) / sizeof(arr_gears_rear[0]); i++) \
            { \
                bike_config.gear_rear.emplace_back(arr_gears_rear[i]); \
            } \
        } \
    } while (0)

#endif // BIKE_CONFIG_HPP
