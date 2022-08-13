#include "bc_test.h"
#include <pico/time.h>
#include <pico/stdio.h>
#include <pico/stdlib.h>

#include <iostream>
#include <string.h>

#include "sim868/interface.hpp"
#include "sim868/gps.hpp"

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
        auto expected_res = "0,70,3961\r\n";


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
            "1,0,20220723071332.000,,,,0.24,0.0,0,,,,,,8,0,,,42,,\r\n";

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
        const size_t expected_no_arr_size = 21;
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

void run_tests(void)
{
    stdio_init_all();
    tracesSetup();
    // BC_TEST_START();
    BC_TEST(test);
    BC_TEST(gps_data_test);
    BC_TEST_END();
}