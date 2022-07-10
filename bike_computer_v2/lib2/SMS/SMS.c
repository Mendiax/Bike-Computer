/*****************************************************************************
* | File      	:   SMS.c
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
#include "SMS.h"

void SMS(char *phone_num, char *sms_info)
{
    char CMGS[] = "AT+CMGS=\"";
    char a[] = "\"";
    char b[] = "1A",b1[]="0";
    memset(b1, 0, sizeof(b1));
    Hexstr_To_str(b, b1, (strlen(b) - 1));
    sendCMD_waitResp("AT+CMGF=1", "OK", 3000);
    strcat(CMGS, phone_num);
    strcat(CMGS, a);
    if (sendCMD_waitResp(CMGS, ">", 2000));
    {
        uart_puts(UART_ID0, sms_info);
        uart_puts(UART_ID0, b1);
    }
}
