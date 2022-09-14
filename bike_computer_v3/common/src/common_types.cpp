#include "common_types.h"

#include <pico/multicore.h>
#include <pico/sync.h>

#include <math.h>


void add_millis(TimeS& time, uint64_t millis_to_add)
{
    TimeS correct_time;
    correct_time.seconds = ((float)millis_to_add / 1000.0f) + time.seconds;
    uint64_t minutes_to_add = std::floor(correct_time.seconds / 60.0f);
    correct_time.seconds -= (float)minutes_to_add * 60.0f;
    //massert(correct_time.seconds < 60.0, "to many seconds %f\n", correct_time.seconds);

    minutes_to_add += time.minutes;
    uint64_t hours_to_add = minutes_to_add / 60;
    minutes_to_add = minutes_to_add % 60;
    //massert(minutes_to_add < 60, "to many minutes %" PRIu64 "\n", minutes_to_add);
    correct_time.minutes = minutes_to_add;

    hours_to_add += time.hour;
    uint64_t days_to_add = hours_to_add / 24;
    hours_to_add = hours_to_add % 24;
    //massert(hours_to_add < 24, "to many hours %" PRIu64 "\n", hours_to_add);
    correct_time.hour = hours_to_add;

    correct_time.day = time.day + days_to_add; // TODO

    // TODO month, yaer
    correct_time.month = time.month;
    correct_time.year = time.year;

    time = correct_time;
}

void TimeS::update_time(absolute_time_t current)
{
    auto millis = us_to_ms(absolute_time_diff_us(this->time_stamp, current));
    if(millis <= 0)
    {
        return;
    }
    add_millis(*this, millis);
    this->time_stamp = current;
}