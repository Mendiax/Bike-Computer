#ifndef __COMMON_GEAR_SUGGESTION_HPP__
#define __COMMON_GEAR_SUGGESTION_HPP__
// #-------------------------------#
// |           includes            |
// #-------------------------------#
// pico includes

// c/c++ includes
#include <limits>


// my includes
#include "bike_config.hpp"

// #-------------------------------#
// |            macros             |
// #-------------------------------#

#define GEAR_SUGGESTION_LEN 2

// #-------------------------------#
// | global types declarations     |
// #-------------------------------#

template<typename T>
static constexpr bool val_in_range(T val,T range_low,T range_high)
{
    return val >= range_low && val <= range_high;
}

enum class Gear_Suggestion{
    UP_SHIFT, DOWN_SHIFT, NO_SHIFT
};

// output
struct Gear_Suggestion_Data
{
    Gear_Suggestion suggestion;
    float cadence_min;
    float cadence_max;
};

void calc_gear(const float& speed, const float& cadence, const Bike_Config& gears);


class Gear_Suggestion_Calculator
{
    const Bike_Config& gears;

    const float cadence_min;
    const float cadence_max;

    inline bool get_too_slow_cadence_range(float cadence)
    {
        return val_in_range((float)cadence, 0.0f, this->cadence_min - 1.0f);
    }

    inline bool get_optimal_cadence_range(float cadence)
    {
        return val_in_range(cadence, this->cadence_min, this->cadence_max);
    }

    inline bool get_optimal_too_fast_cadence_range(float cadence)
    {
        return val_in_range(cadence, this->cadence_min, std::numeric_limits<float>::max());
    }

    inline bool get_too_fast_cadence_range(float cadence)
    {
        return val_in_range(cadence, this->cadence_max + 1.0f, std::numeric_limits<float>::max() );
    }

    float get_current_ratio(const Gear_S& current_gear)
    {
        return gears.get_gear_ratio(current_gear);
    }

    float get_next_ratio(const Gear_S& current_gear)
    {
        const auto prev_gear = gears.get_next_gear(current_gear);
        return gears.get_gear_ratio(prev_gear);
    }

    float get_prev_ratio(const Gear_S& current_gear)
    {
        const auto prev_gear = gears.get_prev_gear(current_gear);
        return gears.get_gear_ratio(prev_gear);
    }

    float get_next_cadence(const Gear_S& current_gear, float cadence)
    {
        if (Bike_Config::is_gear_null(gears.get_next_gear(current_gear)))
        {
            return 0.0f;
        }
        const auto current_r = this->get_current_ratio(current_gear);
        const auto next_r = this->get_next_ratio(current_gear);
        return next_r / current_r * cadence;
    }

    float get_prev_cadence(const Gear_S& current_gear, float cadence)
    {
        if (Bike_Config::is_gear_null(gears.get_prev_gear(current_gear)))
        {
            return 0.0f;
        }
        const auto current_r = this->get_current_ratio(current_gear);
        const auto prev_r = this->get_prev_ratio(current_gear);
        return prev_r / current_r * cadence;
    }

public:
    Gear_Suggestion_Calculator(const Bike_Config& gears)
    : gears{gears}, cadence_min{(float)gears.cadence_min}, cadence_max{(float)gears.cadence_max}
    {
    }

    Gear_Suggestion_Data get_suggested_gear(const float cadence, const Gear_S& current_gear)
    {
        Gear_Suggestion_Data gear_suggestions
            {
                Gear_Suggestion::NO_SHIFT,
                this->cadence_min,
                this->cadence_max
            };
        if (this->get_too_slow_cadence_range(cadence) && this->get_prev_ratio(current_gear) != 0.0)
        {
            gear_suggestions.suggestion = Gear_Suggestion::DOWN_SHIFT;
        }
        else if (this->get_too_fast_cadence_range(cadence)
                && (this->get_optimal_too_fast_cadence_range(this->get_next_cadence(current_gear, cadence))))
        {
            gear_suggestions.suggestion = Gear_Suggestion::UP_SHIFT;
        }

        return gear_suggestions;
    }
};


// #-------------------------------#
// | global variables declarations |
// #-------------------------------#

// #-------------------------------#
// | global function declarations  |
// #-------------------------------#

#endif
