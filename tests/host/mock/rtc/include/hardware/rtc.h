#ifndef __RTC_RTC_HPP__
#define __RTC_RTC_HPP__
// #-------------------------------#
// |           includes            |
// #-------------------------------#
// pico includes
#include "pico/time.h"
// c/c++ includes

// my includes
// #-------------------------------#
// |            macros             |
// #-------------------------------#

// for now do not use
void rtc_init();
void rtc_set_datetime(datetime_t* time_p);
void rtc_get_datetime(datetime_t* time_p);


// #-------------------------------#
// | global types declarations     |
// #-------------------------------#

// #-------------------------------#
// | global variables declarations |
// #-------------------------------#

// #-------------------------------#
// | global function declarations  |
// #-------------------------------#



#endif
