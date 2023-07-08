#include "bc_test.h"
// #include <pico/time.h>
#include <pico/stdio.h>
#include <pico/stdlib.h>

#include <string.h>

#include "parser.hpp"
#include "utils.hpp"
#include "traces.h"



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
    return 0;
}

int url_test(void)
{
    TimeIso8601S sunset{2022,8,9,19,54};
    auto iso_format = get_iso_format(sunset);
    //std::cout << iso_format << std::endl;
    BC_CHECK(iso_format.compare("2022-08-09") == 0);

    return 0;
}

int main()
{
    // stdio_init_all();
    traces_init();

    // FLAKY
    // BC_TEST(parser_test);

    BC_TEST(url_test);


    // BC_TEST(eprom_test);


    BC_TEST_END();
    return bc_get_result() ? 0 : 1;
}