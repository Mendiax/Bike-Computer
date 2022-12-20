#include "session.hpp"
#include "utils.hpp"
#include "speedometer/speedometer.hpp"
#include "traces.h"
#include "massert.hpp"

#include <sstream>
#include <math.h>

Session_Data::Session_Data()
{
    memset(&this->time_start, 0, sizeof(this->time_start));
    memset(&this->time_end, 0, sizeof(this->time_end));
    memset(&this->speed, 0, sizeof(this->speed));

    memset(&this->absolute_time_start, 0, sizeof(this->absolute_time_start));
    memset(&this->absolute_time_last_stop, 0, sizeof(this->absolute_time_last_stop));


    this->status = Session_Data::Status::NOT_STARTED;
}
void Session_Data::start(TimeS time)
{
    if(this->status != Session_Data::Status::NOT_STARTED)
    {
        return;
    }
    this->reset();
    this->time_start = time;

    this->absolute_time_start = get_absolute_time();

    // set to stop
    this->status = Session_Data::Status::RUNNING;
    this->pause();
}
void Session_Data::pause()
{
    switch (this->status)
    {
        case Session_Data::Status::RUNNING:
            {
                this->status = Session_Data::Status::PAUSED;
                this->absolute_time_last_stop = get_absolute_time();
            }
            break;
        case Session_Data::Status::NOT_STARTED:
        case Session_Data::Status::PAUSED:
        case Session_Data::Status::ENDED:
        default:
            break;
    }
}
void Session_Data::cont()
{
    switch (this->status)
    {
        case Session_Data::Status::PAUSED:
            {
                this->status = Session_Data::Status::RUNNING;
                auto stop_time_ms = us_to_ms(absolute_time_diff_us(this->absolute_time_last_stop, get_absolute_time()));
                this->speed.stop_time += stop_time_ms;
            }
            break;
        case Session_Data::Status::NOT_STARTED:
        case Session_Data::Status::RUNNING:
        case Session_Data::Status::ENDED:
        default:
            break;
    }
}
void Session_Data::end(TimeS time)
{
    this->pause();
    this->status = Session_Data::Status::ENDED;
    this->time_end = time;
}

void Session_Data::update(float speed_kph, float distance_m)
{
    uint32_t stop_time = this->speed.stop_time;
    switch (this->status)
    {
    case Session_Data::Status::NOT_STARTED:
    case Session_Data::Status::ENDED:
        return;
    case Session_Data::Status::PAUSED:
        stop_time += us_to_ms(absolute_time_diff_us(this->absolute_time_last_stop, get_absolute_time()));
        break;
    case Session_Data::Status::RUNNING:
        this->speed.velocity = speed_kph;
        this->speed.distance = distance_m / 1000.0;
        this->speed.distanceDec = (uint64_t)(distance_m / 10.0) % 100;
        this->speed.velocityMax = std::fmax(this->speed.velocityMax, this->speed.velocity);
        break;
    }
    auto time_ms = to_ms_since_boot(get_absolute_time());

    const double drive_time_s = ((time_ms - to_ms_since_boot(this->absolute_time_start)) - stop_time) / 1000.0;
    this->speed.drive_time = drive_time_s;
    if (drive_time_s > 0.0)
    {
        this->speed.avg = speed_mps_to_kmph((double)distance_m / drive_time_s);
    }
}

const char *Session_Data::get_header()
{
    return "id;time_start;time_end;"
            "velocityMax;avg;distance;drive_time" // speed
            "\n";
}

Session_Data::Session_Data(const char* csv_line)
{
    const auto data = split_string(csv_line, ';');
    massert(data.size() >= (8), "wrong read session from line:'%s', data size:%zu", csv_line, data.size());
    this->id = std::atoi(data.at(0).c_str());
    this->time_start = time_from_str(data.at(1).c_str());
    this->time_end = time_from_str(data.at(2).c_str());
    this->speed.velocityMax = std::atof(data.at(3).c_str());
    this->speed.avg = std::atof(data.at(4).c_str());
    uint64_t dist = std::atoll(data.at(5).c_str());
    this->speed.distance = dist / 1000;
    this->speed.distanceDec = (dist - speed.distance) / 10;
    this->speed.drive_time = std::atoll(data.at(6).c_str());
}

std::string Session_Data::get_line()
{
    // 12;2004.01.01,00:00:19.50;2004.01.01,00:01:1.59;25.1481;25.3401;22.8785;260;37000  // min len 78 + 8 + ? -> 100 (safe?) dist(m) + time(s?)
    std::stringstream csv_line;
    csv_line << id << ";"
             << time_to_str(time_start) << ";" << time_to_str(time_end) << ";"
             << speed.velocityMax << ";" << speed.avg << ";"
             << (speed.distance * 1000 + speed.distanceDec * 10) << ";" << speed.drive_time << ";\n";

    return csv_line.str();
}
