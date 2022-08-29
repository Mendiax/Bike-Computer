#include "sim868/gps.hpp"
#include "sim868/interface.hpp"
#include "massert.h"
#include "traces.h"

#include <string.h>
#include <inttypes.h>
#include <math.h>


static GpsState gps_current_state = GpsState::OFF;
static GpsRawData gps_last_correct_data;
static GpsRawData gps_last_data;
static bool gps_has_correct_data;
static bool gps_has_correct_date;


void time_print(const TimeS& time)
{
    PRINTF("time: "
        "%4" PRIu16 "\t"
        "%2" PRIu8 "\t"
        "%2" PRIu8 "\t"
        "%2" PRIu8 "\t"
        "%2" PRIu8 "\t"
        "%6.3f \n",
        time.year,
        time.month,
        time.day,
        time.hour,
        time.minutes,
        time.seconds);
}

void get_time_from_str(TimeS& time, const std::string& str)
{
    //PRINTF("date stre = '%s'\n", str.c_str());
    if(str.length() < strlen("20220812130030.000"))
    {
        return;
    }
    // yyyyMMddhhmmss.sss
    
    time.year = std::atoi(str.substr(0,4).c_str());
    time.month = std::atoi(str.substr(4,2).c_str());
    time.day = std::atoi(str.substr(6,2).c_str());
    time.hour = std::atoi(str.substr(8,2).c_str());
    time.minutes = std::atoi(str.substr(10,2).c_str());
    time.seconds = std::atof(str.substr(12).c_str());
    //time_print(time);
}

GpsState sim868::gps::get_gps_state(void)
{
    return gps_current_state;
}


GpsRawData sim868::gps::get_gps_from_respond(const std::string& respond)
{
    /*
    <GNSS run status>,<Fix status>,<UTC date & Time>, <Latitude>,<Longitude>,<MSL Altitude>,
    <Speed Over Ground>,<Course Over Ground>,<Fix Mode>,<Reserved1>,<HDOP>,<PDOP>,<VDOP>,
    <Reserved2>,<GNSS Satellites in View>,<GNSS Satellites Used>,<GLONASS Satellites Used>,
    <Reserved3>,<C/N0 max>,<HPA>,<VPA>
    */
    #define DOC_IDX(idx) (idx-1)
    // if empty return 0
    #define EMPTY(str) strcmp(str,"") == 0 ? "0" : str
    #define EXTRACT_DOC_IDX(data, idx) EMPTY(data[DOC_IDX(idx)].c_str()) 
    PRINTF("str = %s\n", respond.c_str());
    auto data_arr = sim868::split_string(respond);

    GpsRawData data_from_gps = {0};
    data_from_gps.data_time_stamp = get_absolute_time();
    // TODO error handling
    if(data_arr.size() < 20)
    {
        TRACE_ABNORMAL(TRACE_SIM868, " bad respond data_arr.size() = %zu\n", data_arr.size());
        return data_from_gps;
    }
    data_from_gps.status = std::atoi(EXTRACT_DOC_IDX(data_arr, 1));
    get_time_from_str(data_from_gps.utc_date_time, data_arr[DOC_IDX(3)].c_str());
    data_from_gps.latitude = std::atof(EXTRACT_DOC_IDX(data_arr, 4));
    data_from_gps.longitude = std::atof(EXTRACT_DOC_IDX(data_arr, 5));
    data_from_gps.msl_m = std::atoi(EXTRACT_DOC_IDX(data_arr, 6));
    data_from_gps.speed_kph = std::atof(EXTRACT_DOC_IDX(data_arr, 7));
    data_from_gps.gps_satelites = std::atoi(EXTRACT_DOC_IDX(data_arr, 15));
    data_from_gps.gnss_satelites = std::atoi(EXTRACT_DOC_IDX(data_arr, 16));
    data_from_gps.glonass_satelites = std::atoi(EXTRACT_DOC_IDX(data_arr, 17));
    #undef DOC_IDX

    return data_from_gps;
}

void sim868::gps::turn_on()
{
    static uint64_t id;
    if(check_response(id)) // it will return false if id == 0
    {
        auto respond = get_respond(id);
        if(sim868::is_respond_ok(respond))
        {
            gps_current_state = GpsState::NO_SIGNAL;
            //PRINTF("GPS ON\n");
        }
        id = 0;
    }
    else if(id == 0)
    {
        id = send_request("AT+CGNSPWR=1",100);
    }
}

void sim868::gps::turn_off()
{
    static uint64_t id;
    if(check_response(id)) // it will return false if id == 0
    {
        auto respond = get_respond(id);
        if(sim868::is_respond_ok(respond))
        {
            gps_current_state = GpsState::OFF;
            //PRINTF("GPS OFF\n");
        }
        id = 0;
    }
    else if(id == 0)
    {
        id = send_request("AT+CGNSPWR=0",100);
    }
}

bool sim868::gps::fetch_data()
{
    static uint_fast16_t fail_counter;
    // keep gps on
    if(get_gps_state() == GpsState::OFF)
    {
        turn_on();
        return false;
    }
    if(get_gps_state() == GpsState::RESTARTING)
    {
        turn_off();
        fail_counter = 0;
        return false;
    }
    static uint64_t id;
    if(check_response(id)) // it will return false if id == 0
    {
        auto respond = get_respond(id);
        if(sim868::is_respond_ok(respond))
        {
            sim868::clear_respond(respond);
            auto gsm_data = get_gps_from_respond(respond);
            //PRINTF("gsm_data.utc_date_time: ");
            //time_print(gsm_data.utc_date_time);
            gps_current_state = GpsState::NO_RESPOND;
            if(gsm_data.status == 1)
            {
                gps_current_state = GpsState::NO_SIGNAL;
                //PRINTF("respond OK\n");
            }
            if(gsm_data.utc_date_time.year > 2000)
            {
                gps_current_state = GpsState::DATA_AVAIBLE;
                gps_last_data = gsm_data;
                gps_has_correct_date = true;
                //PRINTF("date OK\n");
            }
            if(gsm_data.latitude != 0 && gsm_data.latitude != 0)
            {
                // signal ok
                gps_last_correct_data = gsm_data;
                gps_has_correct_data = true;
                gps_current_state = GpsState::POSITION_AVAIBLE;
                //PRINTF("position OK\n");
            }
        }
        else
        {
            gps_current_state = GpsState::NO_RESPOND;
        }
        if(gps_current_state != GpsState::POSITION_AVAIBLE)
        {
            fail_counter++;
            if(fail_counter >= 10)
            {
                gps_current_state = GpsState::RESTARTING;
            }
        }
        id = 0;
        return true;
    }
    else if(id == 0)
    {
        id = send_request("AT+CGNSINF",2000);
    }
    return false;
}

// void sim868::gps::check_satelites()
// {

// }
// void sim868::gps::check_date()
// {

// }

bool sim868::gps::get_speed(float& speed)
{
    if(!gps_has_correct_data)
    {
        return false;
    }
    speed = gps_last_correct_data.speed_kph;
    return true;
}

bool sim868::gps::get_position(float& latitude, float& longitude)
{
    if(!gps_has_correct_data)
    {
        return false;
    }
    latitude = gps_last_correct_data.latitude;
    longitude = gps_last_correct_data.longitude;

    return true;
}

bool sim868::gps::get_signal(uint8_t& sat, uint8_t& sat2)
{
    // if(!gps_has_correct_data)
    // {
    //     return false;
    // }
    sat = gps_last_data.gps_satelites;
    sat2 = gps_last_data.gnss_satelites;

    return true;
}

bool sim868::gps::get_msl(float& msl)
{
    if(!gps_has_correct_data)
    {
        return false;
    }
    msl = gps_last_correct_data.msl_m;

    return true;
}


bool sim868::gps::get_date(TimeS& time)
{
    if(!gps_has_correct_date)
    {
        return false;
    }
    // PRINTF("get_date.utc_date_time: ");
    // time_print(gps_last_data.utc_date_time);
    //time = gps_last_data.utc_date_time;
    time = correct_time(gps_last_data);
    // PRINTF("return time           : ");
    // time_print(time);
    return true;

}

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

TimeS correct_time(const GpsRawData& gps_data)
{
    auto current_time = gps_data.utc_date_time;

    uint64_t millis_to_add = us_to_ms(absolute_time_diff_us(gps_data.data_time_stamp, get_absolute_time()));
    add_millis(current_time, millis_to_add);
    
    add_millis(current_time, 7200 * 1000); // time offset

    return current_time;
}
