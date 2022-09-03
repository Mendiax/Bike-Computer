#include "session.hpp"

#include <sstream>
#include "common_utils.hpp"

Session::Session()
{

}
void Session::start(TimeS time_start)
{

}


void Session::end(const TimeS& time_end, const SpeedData& data)
{
    this->time_end = time_end;
    this->duration = time_from_millis(data.drive_time * 1000);
    this->velocity_max = data.velocityMax;
    this->velocity_avg = data.avg;
    this->velocity_avg_global = data.avg_global;
    this->distance = (int32_t)data.distance * 1000 + (int32_t)data.distanceDec * 10;
}
std::string Session::to_string()
{
    return "";
}

const char* Session::get_header()
{
    return "time_start;time_end;duration;velocity_max;velocity_avg;velocity_avg_global;distance\n";
}

std::string Session::get_line()
{
    std::stringstream csv_line;
    csv_line << time_to_str(time_start) << ";" << time_to_str(time_end) << ";"
             << time_to_str(duration) << ";" << velocity_max << ";" << velocity_avg << ";"
             << velocity_avg_global << ";" << distance << "\n";

    return csv_line.str();
}