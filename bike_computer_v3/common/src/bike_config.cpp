#include "bike_config.hpp"
#include "traces.h"
#include <string.h>
#include <cstdlib>

uint8_t Bike_Config_S::to_idx(const Gear_S& gear)
{
    return gear.front * this->gear_rear.size() + gear.rear;
}

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


        if(check_floats_prec(gear_ratio, ratio, 0.08))
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

