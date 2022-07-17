/*****************************************************************************
* | File      	:	Pico-SIM868-Test.h
* | Author      :   Waveshare team
* | Function    :   e-Paper test Demo
* | Info        :
*----------------
* |	This version:   V1.0
* | Date        :   2022-02-28
* | Info        :   
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to  whom the Software is
# furished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
******************************************************************************/
#ifndef _Pico_SIM868_Test_H_
#define _Pico_SIM868_Test_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "DEV_GSM_Config.h"
#include"AT_Test.h"
#include"BT.h"
#include"GPS.h"
#include"HTTP.h"
#include"Phone_Call.h"
#include "SMS.h"



int at_test();
int BT_test();
int GPS_test();
int HTTP_test();
int phone_call_test();
int SMS_test();
#ifdef __cplusplus
 }
#endif
// int Pico_SIM7020X_NB_loT_HTTP();
// int Pico_SIM7020X_NB_loT_MQTT();
// int Pico_SIM7020X_NB_loT_TCP();
#endif
