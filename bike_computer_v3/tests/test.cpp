#include "bc_test.h"
#include <pico/time.h>
#include <pico/stdio.h>
#include <pico/stdlib.h>

#include <string.h>


#include "sim868/interface.hpp"
#include "sim868/gps.hpp"
#include "common_utils.hpp"
#include "sim868/gsm.hpp"

#include "sd_file.h"
#include "ff.h"
#include "session.hpp"
#include "sd_common.h"

#include "BMP280.hpp"

#include "parser.hpp"

#include "massert.h"
#include "traces.h"

#include "test_actors.h"


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
        auto data_arr = split_string(gps_respond);
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

int altitude_test(void)
{
    float temperature_2m = 16.4;
    float pressure_msl = 1017.3;
    float surface_pressure = 1004.2;

    auto h = bmp280::get_height(pressure_msl, surface_pressure, temperature_2m);
    PRINTF("altitude = %f\n", h);
    BC_CHECK(h >= 109.0 && h <= 110.0);

    return 0;
}

int sd_drive_test(void)
{
    const char* file_name = "sd_test.csv";
    mount_drive();
    f_unlink(file_name);
    Sd_File file(file_name);
    PRINTF("File opened\n");
    //file.clear();
    PRINTF("File cleared\n");


    // TimeS time_start{2022,8,9,19,54,13.01};
    // TimeS time_end{2022,8,9,20,54,13.01};

    // SpeedData data{};

    // data.drive_time = 3650;
    // data.velocityMax = 30;
    // data.avg = 20;
    // data.avg_global = 15;
    // data.distance = 13;
    // data.distanceDec = 11;

    // Session session;
    // session.start(time_start);
    // session.end(time_end, data);

    const char* header_line = "time_start;time_end;duration;velocity_max;velocity_avg;velocity_avg_global;distance\n";
    const char* first_line = "16720.83.83,69:68:6.56e;2022.08.09,20:54:13.01;01:00:50.00;30.0000;20.0000;15.0000;13110\n";
    PRINTF("header_line: %s\n", header_line);
    PRINTF("first_line: %s\n", first_line);

    if(file.is_empty())
    {
        file.append(header_line);
    }
    file.append(first_line);
    {
        const size_t file_size = strlen(header_line) + strlen(first_line);
        const size_t get_size = file.get_size();
        BC_CHECK_EQ(get_size, file_size);
    }
    PRINTF("File written\n");


    FIL fp;
    auto res = f_open(&fp, file_name, FA_OPEN_EXISTING | FA_WRITE | FA_READ);
    BC_CHECK_VERBAL(res == FR_OK, "%d", res);
    PRINTF("Open ok\n");


    //res = f_truncate(&fp);
    res = f_rewind(&fp);
    BC_CHECK_VERBAL(res == FR_OK, "%d", res);
    PRINTF("Rewind ok\n");

    FILINFO info;
    auto fres = f_stat(file_name, &info);
    PRINTF("file size == %" PRIu64 " \n", info.fsize);



    {
        auto test_string = header_line;
        enum{BUFFER_SIZE=256};
        char buffer[BUFFER_SIZE] = {0};
        UINT bytes_read = 0;
        PRINTF("Reading\n");
        auto res = f_read(&fp, buffer, strlen(test_string), &bytes_read);
        PRINTF("Reading ok\n");
        BC_CHECK_VERBAL(res == FR_OK, "%d", res);
        PRINTF(" read: %s\n", buffer);
        BC_CHECK_VERBAL(bytes_read == strlen(test_string), "%d", bytes_read);
        BC_CHECK(strcmp(test_string, buffer) == 0);
    }

    {
        auto test_string = first_line;
        enum{BUFFER_SIZE=256};
        char buffer[BUFFER_SIZE] = {0};
       // auto res = f_gets(buffer, BUFFER_SIZE, &fp);
        // BC_CHECK_VERBAL(res != NULL, "%d", res);
        // printf(" read: %s\n", res);
        // BC_CHECK(strcmp(test_string, res) == 0);
        UINT bytes_read = 0;
        PRINTF("Reading\n");
        auto res = f_read(&fp, buffer, strlen(test_string), &bytes_read);
        BC_CHECK_VERBAL(res == FR_OK, "%d", res);
        BC_CHECK_VERBAL(bytes_read == strlen(test_string), "%d", bytes_read);
        PRINTF("Reading ok\n");
        //PRINTF(" read: %s\n", buffer);
        BC_CHECK(strcmp(test_string, buffer) == 0);
        PRINTF("Reading ok2\n");

    }
    f_close(&fp);
    PRINTF("Close\n");

    const std::string file_content_expected = std::string(header_line) + std::string(first_line);
    const std::string file_content = file.read_all();
    BC_CHECK_EQ(file_content_expected, file_content);

    file.clear();
    {
        const size_t file_size = 0;
        const size_t get_size = file.get_size();
        BC_CHECK_EQ(get_size, file_size);
    }
    {
        const std::string content = "1234567890";
        file.append(content.c_str());
        {
            const size_t file_size = content.length();
            const size_t get_size = file.get_size();
            BC_CHECK_EQ(get_size, file_size);
        }

        const std::string second_content = "0987654321";
        file.append(second_content.c_str());
        // file = content second_content
        BC_CHECK(content.length() == second_content.length());


        {
            const std::string file_content_expected = content + second_content;
            const std::string file_content = file.read_all();
            BC_CHECK_EQ(file_content_expected, file_content);
        }
        file.overwrite(second_content.c_str(), 0);
        {
            const std::string file_content_expected = second_content + second_content;
            const std::string file_content = file.read_all();
            BC_CHECK_EQ(file_content_expected, file_content);
        }
        file.overwrite(content.c_str(), second_content.length());
        {
            const std::string file_content_expected = second_content + content;
            const std::string file_content = file.read_all();
            BC_CHECK_EQ(file_content_expected, file_content);
        }
    }

    file.remove();
    return 0;
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
    BC_TEST(altitude_test);
    //sd_drive_test();
    BC_TEST(sd_drive_test);

    //BC_TEST(test_actors);

    BC_TEST_END();
}