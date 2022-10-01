#include "bike_config.hpp"
#include "common_utils.hpp"
#include "massert.hpp"
#include "traces.h"

#include <string.h>
#include <cstdlib>

class Gear_Iterator
{
    uint8_t current_front;
    uint8_t current_rear;
    const Bike_Config_S& config;
public:
    Gear_Iterator(const Bike_Config_S& config);
    bool has_next();
    float get_next();
    Gear_S get_gear();
};

Gear_Iterator::Gear_Iterator(const Bike_Config_S& config)
    :config{config},current_front{0}, current_rear{0}
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
    return {current_front, current_rear};
}

#define GEARS_FRONT_STR "GF"
#define GEARS_REAR_STR "GR"
#define WHEEL_SIZE_STR "WS"



const char* Bike_Config_S::to_string()
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

void Bike_Config_S::from_string(const char* str)
{
    this->wheel_size = 2.0; // default

    std::istringstream iss(str);
    std::string line;
    while (std::getline(iss, line)) {
        // PRINTF("line %s\n", line.c_str());
        const auto line_arr = split_string(line, ':');
        if(line_arr.size() < 2) {
            continue;
        }
        // PRINTF("[0] = %s\n", line_arr.at(0).c_str());
        // PRINTF("[1] = %s\n", line_arr.at(1).c_str());
        if(line_arr.at(0).compare(GEARS_FRONT_STR) == 0)
        {
            // PRINTF( GEARS_FRONT_STR "\n");
            auto gears = split_string(line_arr.at(1), ',');
            // TRACE_DEBUG(1, TRACE_BIKE_CONFIG, " read front gears size %zu\n", gears.size());

            gear_front.clear();
            for(auto&& gear_str : gears)
            {
                // PRINTF( " gear %s\n", gear_str.c_str());
                gear_front.emplace_back(std::atoi(gear_str.c_str()));
            }
            // TRACE_DEBUG(1, TRACE_BIKE_CONFIG, " read front gears size %zu\n", gears.size());

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
            // PRINTF( WHEEL_SIZE_STR "\n");
            wheel_size = std::atof(line_arr.at(1).c_str());
        }
    }

    // setup gear diff
    float gear_diff = 100.0; // TODO add define
    Gear_Iterator iter(*this);
    float last_gear_ratio = 0.0f;
    while (iter.has_next())
    {
        float gear_ratio = iter.get_next();
        gear_diff = std::min(std::abs(last_gear_ratio - gear_ratio), gear_diff);
        last_gear_ratio = gear_ratio;
    }
    TRACE_DEBUG(2, TRACE_BIKE_CONFIG, "min diff gear ratio: %f\n", gear_diff);
    this->min_gear_diff = gear_diff * 0.99f; // add some space for errors
}

/**
 * @brief checks if f2 is about f1 with precision
 *
 * @param f1
 * @param f2
 * @param prec
 * @return true
 * @return false
 */
static inline bool check_floats_prec(float f1, float f2, float prec);

Gear_S Bike_Config_S::get_current_gear(float ratio)
{
    Gear_Iterator iter(*this);
    while (iter.has_next())
    {
        Gear_S gear = iter.get_gear();
        float gear_ratio = iter.get_next();
        /*
        my gears:
        [2.909090909090909, 2.4615384615384617, 2.1333333333333333,
         1.7777777777777777, 1.5238095238095237, 1.3333333333333333,
         1.1428571428571428, 0.9696969696969697, 0.8205128205128205,
         0.7111111111111111, 0.6274509803921569]
        */

       // TODO add algoriths that work better at reading gear (precision) ??? now we tak minimum
        if(check_floats_prec(gear_ratio, ratio, this->min_gear_diff))
        {
            gear.front += 1;
            gear.rear += 1;
            return gear;
        }
        // TRACE_DEBUG(5, TRACE_CORE_0,
        //     "ratio=%f, gear_ratio=%f, gear={%" PRIu8 ",%" PRIu8 "}\n",
        //     ratio, gear_ratio, gear.front + 1, gear.rear + 1);
    }
    return {0,0};
}

float Bike_Config_S::get_gear_ratio(Gear_S gear)
{
    return (float)this->gear_front.at(gear.front - 1) / (float) this->gear_rear.at(gear.rear - 1);
}


static inline bool check_floats_prec(float f1, float f2, float prec)
{
    // TRACE_DEBUG(5, TRACE_CORE_0,
    //         "%d, %d\n",
    //         (f1 - prec <= f2), (f1 + prec >= f2));
    return ((f1 - prec <= f2) &&
            (f1 + prec >= f2));
}

#define VAL(str) #str
#define TOSTRING(str) VAL(str)

std::string Gear_Usage::to_string()
{
    std::string header = "[" TOSTRING(MAX_NO_OF_GEARS) "|" TOSTRING(CADENCE_DATA_MIN) "|" TOSTRING(CADENCE_DATA_MAX) "|" TOSTRING(CADENCE_DATA_DELTA) "]";
    for(int i = 0; i < MAX_NO_OF_GEARS; i++)
    {
        for (int j = 0; j < CADENCE_DATA_LEN; j++)
        {
            header += ",";
            header += std::to_string(usage[i][j]);
        }
    }
    return header;
}

void Gear_Usage::from_string(const std::string& str)
{
    // auto fp = str.find_first_of('[');
    // auto lp = str.find_first_of(']');

    // auto len = fp - lp + 1;
    // auto header_str = str.substr(lp, len);

    // TODO optimize memory usage

    auto start_pos = str.find_first_of(']') + 1;

    auto data_arr = split_string(str.substr(start_pos),',');

    for(int i = 0; i < MAX_NO_OF_GEARS; i++)
    {
        for (int j = 0; j < CADENCE_DATA_LEN; j++)
        {
            const size_t idx = i * CADENCE_DATA_LEN + j;
            massert(idx < data_arr.size(), "gear usage from str failed: %zu\n", idx);
            this->usage[i][j] = std::atof(data_arr[idx].c_str());
        }
    }

}

