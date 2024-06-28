#include "bike_config.hpp"
#include "utils.hpp"
#include "massert.hpp"
#include "traces.h"

#include <string.h>
#include <cstdlib>

class Gear_Iterator
{
    uint8_t current_front;
    uint8_t current_rear;
    const Bike_Config& config;
public:
    Gear_Iterator(const Bike_Config& config);
    bool has_next();
    float get_next();
    Gear_S get_gear();
};

Gear_Iterator::Gear_Iterator(const Bike_Config& config)
    :current_front{0}, current_rear{0},config{config}
{}

bool Gear_Iterator::has_next()
{
    return current_front < config.gear_front.size();
}
float Gear_Iterator::get_next()
{
    float ratio = (float)config.gear_front.at(current_front) /
                  (float)config.gear_rear.at(current_rear++);
    if(current_rear == config.gear_rear.size())
    {
        current_front++;
        current_rear = 0;
    }
    return ratio;
}
Gear_S Gear_Iterator::get_gear()
{
    const uint8_t fr = current_front + 1;
    const uint8_t rr = current_rear + 1;
    return {fr, rr};
}

#define GEARS_FRONT_STR "GEARS_FRONT"
#define GEARS_REAR_STR "GEARS_REAR"
#define WHEEL_SIZE_STR "WHEEL_SIZE"
#define TIME_OFFSET_STR "TIME_OFFSET"
#define CADENCE_MIN_STR "CAD_MIN"
#define CADENCE_MAX_STR "CAD_MAX"
#define NO_MAGNETS_CADENCE_STR "CADENCE_MAGNETS"
#define NO_MAGNETS_SPEED_STR "SPEED_MAGNETS"





Gear_S Bike_Config::get_next_gear(Gear_S gear) const
{
    gear.rear++;
    if(gear.rear <= gear_rear.size())
    {
        return gear;
    }
    // we reached limit of rear gear
    gear.rear = 0;
    gear.front++;
    if(gear.front <= gear_front.size())
    {
        return gear;
    }
    // reached max gear
    return {0,0};
}
Gear_S Bike_Config::get_prev_gear(Gear_S gear) const
{
    if(gear.rear > 1)
    {
        gear.rear--;
        return gear;
    }
    if(gear.front > 1)
    {
        gear.rear = gear_rear.size();
        gear.front--;
        return gear;
    }
    // reached min gear
    return {0,0};
}
float Bike_Config::get_gear_ratio(Gear_S gear) const
{
    if(is_gear_null(gear))
    {
        return 0.0f;
    }
    return (float)gear_front.at(gear.front - 1) / (float)gear_rear.at(gear.rear - 1);
}

// TODO remove
const char* Bike_Config::to_string()
{
    std::stringstream ss;
    ss << GEARS_FRONT_STR ":";
    TRACE_DEBUG(1, TRACE_BIKE_CONFIG, " gear_front size %zu\n", gear_front.size());
    for(auto&& gear :gear_front){ss << (int)gear << ',';}
    ss.seekp(-1,ss.cur); ss << "\n"; //  overwrite last char

    TRACE_DEBUG(1, TRACE_BIKE_CONFIG, " gear_rear size %zu\n", gear_rear.size());
    ss << GEARS_REAR_STR ":"; for(auto&& gear :gear_rear) { ss << (int)gear << ','; }
    ss.seekp(-1,ss.cur); ss << "\n"; //  overwrite last char

    ss << WHEEL_SIZE_STR ":" << wheel_size << '\n';
    TRACE_DEBUG(1, TRACE_BIKE_CONFIG, " config to str:\n%s\n", ss.str().c_str());

    return ss.str().c_str();
}

/*
str example:
GEARS_FRONT:32
GEARS_REAR:51,45,39,33,28,24,21,18,15,13,11
WHEEL_SIZE:2.186484
TIME_OFFSET:+1
CAD_MIN:75
CAD_MAX:90
*/
bool Bike_Config::from_string(const char* str)
{
    // defaults
    this->wheel_size = 2.0;
    this->gear_front.clear();
    this->gear_rear.clear();
    this->no_magnets_cadence = 1;
    this->no_magnets_speed = 1;
    this->cadence_max = 90;
    this->cadence_min = 70;


    std::istringstream iss(str);
    std::string line;
    while (std::getline(iss, line)) {
        // PRINTF("line %s\n", line.c_str());
        const auto line_arr = split_string(line, ':');
        if(line_arr.size() < 2) {
            continue;
        }
        if(line_arr.at(0).compare(GEARS_FRONT_STR) == 0)
        {
            auto gears = split_string(line_arr.at(1), ',');
            gear_front.clear();
            for(auto&& gear_str : gears)
            {
                gear_front.emplace_back(std::atoi(gear_str.c_str()));
            }
        }
        else if(line_arr.at(0).compare(GEARS_REAR_STR) == 0)
        {
            // PRINTF( GEARS_REAR_STR "\n");
            auto gears = split_string(line_arr.at(1), ',');
            gear_rear.clear();
            for(auto&& gear_str : gears)
            {
                gear_rear.emplace_back(std::atoi(gear_str.c_str()));
            }
        }
        else if(line_arr.at(0).compare(WHEEL_SIZE_STR) == 0)
        {
            wheel_size = std::atof(line_arr.at(1).c_str());
        }
        else if(line_arr.at(0).compare(TIME_OFFSET_STR) == 0)
        {
            this->hour_offset = std::atoi(line_arr.at(1).c_str());
        }
        else if(line_arr.at(0).compare(CADENCE_MAX_STR) == 0)
        {
            this->cadence_max = std::atoi(line_arr.at(1).c_str());
        }
        else if(line_arr.at(0).compare(CADENCE_MIN_STR) == 0)
        {
            this->cadence_min = std::atoi(line_arr.at(1).c_str());
        }
        else if(line_arr.at(0).compare(NO_MAGNETS_CADENCE_STR) == 0)
        {
            this->no_magnets_cadence = std::atoi(line_arr.at(1).c_str());
        }
        else if(line_arr.at(0).compare(NO_MAGNETS_SPEED_STR) == 0)
        {
            this->no_magnets_speed = std::atoi(line_arr.at(1).c_str());
        }
    }

    if(gear_front.size() < 1 ||
       gear_rear.size() < 1 ||
       wheel_size == 0.0)
    {
        this->to_string(); // print debug
        return false; // fail
    }


    Gear_Iterator iter(*this);

    const auto gear_len = gear_rear.size();

    std::vector<float> gear_ratios(gear_len);
    size_t i = 0;
    while (iter.has_next())
    {
        float gear_ratio = iter.get_next();
        gear_ratios[i++] = gear_ratio;
    }

    auto get_diff = [&](size_t k)
    {
        return gear_ratios[k + 1] - gear_ratios[k];
    };

    gear_ranges.resize(gear_len);
    gear_ranges[0] = {
        gear_ratios[0] - get_diff(0)/2.0f,
        gear_ratios[0] + get_diff(0)/2.0f,
    };
    for(unsigned i = 1; i < gear_len - 1; i++)
    {
        gear_ranges[i] = {
            gear_ratios[i] - get_diff(i-1)/2.0f,
            gear_ratios[i] + get_diff(i)/2.0f,
        };
    }
     gear_ranges[gear_len - 1] = {
            gear_ratios[gear_len - 1] - get_diff(gear_len-2)/2.0f,
            gear_ratios[gear_len - 1] + get_diff(gear_len-2)/2.0f,
        };

    return true;
}


Gear_S Bike_Config::get_current_gear(float ratio)
{
    for(size_t i = 0; i < gear_ranges.size(); i++)
    {
        if(gear_ranges.at(i).min <= ratio && ratio < gear_ranges.at(i).max)
        {
            return {1, (uint8_t) (i + 1)};
        }
    }
    return {0,0};
}

float Bike_Config::get_gear_ratio(Gear_S gear)
{
    return (float)this->gear_front.at(gear.front - 1) / (float) this->gear_rear.at(gear.rear - 1);
}

