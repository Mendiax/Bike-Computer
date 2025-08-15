#include "common_types.h"
#include <math.h>
#include <stdio.h>
#include <time.h>

int day_of_week(int year, int month, int day) {
    if (month < 3) {
        month += 12;
        year -= 1;
    }
    int K = year % 100;
    int J = year / 100;
    int h = (day + (13 * (month + 1)) / 5 + K + K / 4 + J / 4 + 5 * J) % 7;
    return ((h + 6) % 7); // convert to 0=Sunday
}

// Find last Sunday of a month
int last_sunday(int year, int month) {
    int last_day;
    if (month == 4 || month == 6 || month == 9 || month == 11)
        last_day = 30;
    else if (month == 2) {
        bool leap = (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
        last_day = leap ? 29 : 28;
    } else {
        last_day = 31;
    }
    int dow = day_of_week(year, month, last_day);
    return last_day - dow; // Sunday = 0, so subtract to get Sunday
}

// Returns UTC offset in hours for Poland
int poland_offset_hours(int year, int month, int day, int hour) {
    int start_day = last_sunday(year, 3);  // last Sunday in March
    int end_day   = last_sunday(year, 10); // last Sunday in October

    bool in_summer = false;

    if (month > 3 && month < 10) {
        in_summer = true;
    } else if (month == 3) {
        if (day > start_day || (day == start_day && hour >= 2))
            in_summer = true;
    } else if (month == 10) {
        if (day < end_day || (day == end_day && hour < 3))
            in_summer = true;
    }

    return in_summer ? 2 : 1;
}

double poland_utc_offset_hours(const datetime_t& t) {
    return poland_offset_hours(t.year, t.month, t.day, t.hour);
}

bool TimeS::is_valid() const
{
    return  this->year >= 2022
            && this->month <= 12
            && this->month >= 1
            && this->day <= 31
            && this->day >= 1;
}

datetime_t TimeS::to_date_time() const
{
    datetime_t t{
        (int16_t)this->year,
        (int8_t)this->month,
        (int8_t)this->day,
        (int8_t)0,
        (int8_t)this->hour,
        (int8_t)this->minutes,
        (int8_t)this->seconds
    };
    return t;
}


void TimeS::from_date_time(const datetime_t& date)
{
    this->year = date.year;
    this->month = date.month;
    this->day = date.day;
    // this->dotw = date.dotw;
    this->hour = date.hour;
    this->minutes = date.min;
    this->seconds = date.sec;
}


static void add_millis(TimeS& time, uint64_t millis_to_add)
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

uint32_t extract_seconds(Time_HourS& time);
uint32_t extract_minutes(Time_HourS& time);
uint32_t extract_hours(Time_HourS& time);

uint32_t extract_seconds(Time_HourS& time)
{
    auto ms = time.seconds * 1000.0;
    time.seconds = 0.0;
    return (uint32_t)ms;
}
uint32_t extract_minutes(Time_HourS& time)
{
    auto ms = (uint32_t)time.minutes * 60 * 1000;
    time.minutes = 0;
    return (uint32_t)ms;
}
uint32_t extract_hours(Time_HourS& time)
{
    auto ms = (uint32_t)time.hour * 3600 * 1000;
    time.hour = 0;
    return (uint32_t)ms;
}

uint32_t TimeS::to_ms() const
{
    TimeS copy = *this;
    uint64_t ms = 0;
    ms +=extract_seconds(copy.hours);
    ms += extract_minutes(copy.hours);
    ms += extract_hours(copy.hours);
    return ms;
}


void TimeS::substract_ms(uint32_t ms)
{
    uint64_t ms_to_add = 0;
    if(ms_to_add < ms){
        ms_to_add += extract_seconds(this->hours);
    }
    if(ms_to_add < ms){
        ms_to_add += extract_minutes(this->hours);
    }
    if(ms_to_add < ms){
        ms_to_add += extract_hours(this->hours);
    }
    if(ms_to_add >= ms)
    {
        add_millis(*this, ms_to_add);
    }
}
