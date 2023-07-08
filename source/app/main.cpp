// pico includes
#include <pico/stdlib.h>
#include <string.h>

// c/c++ includes
// my includes
#include "traces.h"
#include "core_setup.h"



std::string read_non_block()
{
    #define ENDSTDIN	255
    #define CR		13
    #define LF		10
    char strg[100] = {0};
    unsigned char chr;
    int lp = 0;

    memset(strg, 0, sizeof(strg));

	chr = getchar_timeout_us(0);
	while(chr != ENDSTDIN)
	{
        if (chr != CR)
		    strg[lp++] = chr;
		if(chr == LF || lp == (sizeof(strg) - 1))
		{
			strg[lp] = 0;	//terminate string
			lp = 0;
			break;
		}

		chr = getchar_timeout_us(0);
	}
    return std::string(strg);
}

int main()
{
    //traces setup
    traces_init();

    // setup console
    sleep_ms(1000);
    stdio_init_all();
    sleep_ms(2000);

    // while (1) {
    //     printf("[START] waiting for \"s\"\n");
    //     sleep_ms(1000);

    //     if (read_non_block() == "s"){
    //         printf("[START] received \"s\"\n");
    //         break;
    //     }
    // }

    TRACE_DEBUG(0, TRACE_MAIN, "Main start\n");
    PRINT("SYSTEM START");

    core_setup();
}

// DO NOT REMOVE !!!
// static float getTemp()
// {
//     float conversion_factor = 3.28f / (1 << 12);
//     adc_select_input(0);
//     float voltage = adc_read() * conversion_factor;

//     const float U = 3.28f;
//     const float R1 = 3270;
//     const float R3 = 1;
//     float Rt = (voltage/(U-voltage)) * R1 - R3;

//     const float T0 = 298.15;
//     const float R0 = 9170;
//     const float B = 4000;
//     const float T = (B * T0)/(T0*std::log(Rt/R0) + B);

//     const int Tc = T - 273.15;


//     std::string log = "temp:" + std::to_string(Tc) + "C";
//     display::println(16 * Font8.width, 0, log.c_str(), &Font8);
//     //__display->write();
//     return voltage;
// }

