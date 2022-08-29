#include "bc_test.h"
#include <pico/time.h>
#include <pico/stdio.h>
#include <pico/stdlib.h>

#include <iostream>
#include <string.h>

#include "sim868/interface.hpp"
#include "sim868/gps.hpp"
#include "common_utils.hpp"
#include "sim868/gsm.hpp"

#include "parser.hpp"

#include "massert.h"
#include "traces.h"

#define BC_CHECK(statement) if(!(statement)){fprintf(stderr, "[ERROR] " #statement " failed\n"); return ERROR;}

void print_c_str(const char* msg)
{
    size_t len = strlen(msg);
    // debuging no LCD console
    printf("number of bytes read: %zu\n", len);
    printf("=====\n");
    for(int j = 0; j < len; j++)
    {
        printf("\'%d\' ", (int)msg[j]);
    }
    printf("\n=====\n");
}

// test function
int test()
{
    {
        // AT+CBC
        // +CBC: 0,70,3961
        //
        // OK
        // 
        std::string respond =
            "AT+CBC\r\n"
            "+CBC: 0,70,3961\r\n"
            "\r\n"
            "OK\r\n";
        if(!sim868::is_respond_ok(respond))
        {
            std::cout << "ERROR: sim868::is_respond_ok failed" << std::endl;
            return ERROR;
        }
    }
    {
        // AT+CBC
        // +CBC: 0,70,3961
        //
        // OK
        // 
        std::string respond =
            "AT+CBC\r\n"
            "+CBC: 0,70,3961\r\n"
            "\r\n"
            "OK\r\n";
        sim868::clear_respond(respond);
        auto expected_res = "0,70,3961";


        if(strcmp(respond.c_str(), expected_res) != 0)
        {
            std::cerr << "ERROR: '" <<  respond << "' != '" << expected_res << "'" << std::endl;
            return ERROR;
        }
    }
    {
        std::string gps_respond =
            "AT+CGNSINF\r\n"
            "+CGNSINF: 1,0,20220723071332.000,,,,0.24,0.0,0,,,,,,8,0,,,42,,\r\n"
            "\r\n"
            "OK\r\n";
        sim868::clear_respond(gps_respond);
        std::string expected_res =
            "1,0,20220723071332.000,,,,0.24,0.0,0,,,,,,8,0,,,42,,";

        if(gps_respond != expected_res)
        {
            printf("respond check %s != %s\n", gps_respond.c_str(), expected_res.c_str());
            return ERROR;
        }
    }


    return 0;
}

int gps_data_test(void)
{
    {
        std::string gps_respond =
            "AT+CGNSINF\r\n"
            "+CGNSINF: 1,0,20220723071332.000,,,,0.24,0.0,0,,,,,,8,0,,,42,,\r\n"
            "\r\n"
            "OK\r\n";
        sim868::clear_respond(gps_respond);
        auto data_arr = sim868::split_string(gps_respond);
        const size_t expected_no_arr_size = 20;
        if(data_arr.size() != expected_no_arr_size)
        {
            printf("gps data size not to spec got: %zu expected: %zu\n", data_arr.size(), expected_no_arr_size);
            return ERROR;
        }
        if(strcmp(data_arr.at(3).c_str(), "") != 0)
        {
            printf("did not get empty string from empty gps data%s \n", data_arr.at(3).c_str());
            return ERROR;
        }
    }
    {
        std::string gps_respond =
            "AT+CGNSINF\r\n"
            "+CGNSINF: 1,0,20220723071332.000,,,,0.24,0.0,0,,,,,,8,0,,,42,,\r\n"
            "\r\n"
            "OK\r\n";
        sim868::clear_respond(gps_respond);
        GpsRawData gps_data = sim868::gps::get_gps_from_respond(gps_respond);
        BC_CHECK(gps_data.status == 1);
        BC_CHECK(gps_data.latitude == 0);
        BC_CHECK(gps_data.longitude == 0);
        BC_CHECK(gps_data.gps_satelites == 8);
        BC_CHECK(gps_data.gnss_satelites == 0);
        BC_CHECK(gps_data.glonass_satelites == 0);
        BC_CHECK(gps_data.utc_date_time.year == 2022);
        BC_CHECK(gps_data.utc_date_time.hour == 07);
        BC_CHECK(gps_data.utc_date_time.minutes == 13);
    }
    return 0;
}

// in file test_data.cpp
extern const char* forecast;
extern const char* day1_json;
extern const char* test_json;

int parser_test(void)
{
    auto p = parse_json(day1_json, 1);
    if(p != NULL)
    {
        assert(p->current_weather.temperature == 26.5f);
        assert(p->current_weather.windspeed == 9.8f);
        assert(p->current_weather.winddirection == 126.0f);
        float temp[] = {22.5, 21.9, 21.8, 21.4, 21.1, 20.8, 20.6, 21.4, 23.2, 25.3, 27.3, 28.7, 29.9, 30.8, 31.4, 31.8, 31.8, 31.6, 30.8, 29.7, 28.0, 26.5, 25.5, 24.7 };
        assert(memcmp(temp, p->temperature_2m, sizeof(temp)) == 0);
        float winddir[] = {115.0, 122.0, 128.0, 128.0, 127.0, 118.0, 117.0, 115.0, 115.0, 117.0, 124.0, 139.0, 139.0, 141.0, 141.0, 144.0, 144.0, 145.0, 146.0, 135.0, 124.0, 126.0, 126.0, 126.0 };
        assert(memcmp(winddir, p->winddirection_10m, sizeof(winddir)) == 0);
        float wind_dom[] = {132.33426};
        assert(memcmp(wind_dom, p->winddirection_10m_dominant, sizeof(wind_dom)) == 0);

        TimeIso8601S sunset{2022,8,19,19,54};
        assert(memcmp(&sunset, &p->sunset[0], sizeof(sunset)) == 0);
        //std::cout << "date check " << memcmp(&sunset, &p->sunset[0], sizeof(sunset)) << std::end;

        delete p;
    }

    {
        auto p = parse_json(day1_json, 1);
        if(p)
        {
            delete p;
        }
    }
    return 0;
}

int url_test(void)
{
    TimeIso8601S sunset{2022,8,9,19,54};
    auto iso_format = get_iso_format(sunset);
    //std::cout << iso_format << std::endl;
    BC_CHECK(iso_format.compare("2022-08-09") == 0);

    Time_DateS date {2022,8,15};
    std::string expected_url = "http://api.open-meteo.com/v1/forecast?latitude=52.5000&longitude=13.5000&hourly=temperature_2m,pressure_msl,precipitation,windspeed_10m,winddirection_10m,windgusts_10m&daily=sunrise,sunset,winddirection_10m_dominant&current_weather=true&timezone=Europe/Berlin&start_date=2022-08-15&end_date=2022-08-15";
    auto url = sim868::gsm::construct_http_request_url(52.5, 13.5,date, date);
    std::cout << "url " << url << std::endl;
    BC_CHECK(expected_url.compare(url) == 0);


    return 0;
}

// extern void Paint_DrawLineGen(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, void (*draw_func)(uint16_t x, uint16_t y, display::DisplayColor color), display::DisplayColor color, uint8_t scale);

int draw_line_test(void)
{
    
}
void run_tests(void)
{
    stdio_init_all();
    tracesSetup();
    // BC_TEST_START();
    BC_TEST(test);
    BC_TEST(gps_data_test);
    BC_TEST(parser_test);
    BC_TEST(url_test);
    BC_TEST_END();
}