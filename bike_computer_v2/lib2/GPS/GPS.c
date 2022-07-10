/*****************************************************************************
* | File      	:   GPS.c
* | Author      :   Waveshare team
* | Function    :   Hardware underlying interface
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
# copies of theex Software, and to permit persons to  whom the Software is
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
#include "GPS.h"
#include "console/console.h"

void get_gps_info()
{
    int count = 0;
    consolep("Start GPS session...\r\n");
    sendCMD_waitResp("AT+CGNSPWR=1", "OK", 2000);
    DEV_GSM_Delay_ms(2000);
    for (int i = 1; i < 10; i++)
    {
        if (sendCMD_waitResp("AT+CGNSINF", ",,,,", 2000) == 1)
        {
            consolep("GPS is not ready\r\n");
            if (i >= 9)
            {
                consolep("GPS positioning failed, please check the GPS antenna!\r\n");
                sendCMD_waitResp("AT+CGNSPWR=0", "OK", 2000);
            }
            else
            {
                consolep("wait...\r\n");
                DEV_GSM_Delay_ms(2000);
                continue;
            }
        }
        else
        {
            if (count <= 3)
            {
                count++;
                consolep("GPS info:\r\n");
            }
            else
            {
                sendCMD_waitResp("AT+CGNSPWR=0", "OK", 2000);
                break;
            }
        }
    }
}
