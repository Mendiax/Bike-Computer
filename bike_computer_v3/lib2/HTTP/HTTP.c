/*****************************************************************************
* | File      	:   HTTP.c
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
#include "HTTP.h"
#include "console/console.h"

void http_get()
{
    char http_get_server0[] = "http://api.seniverse.com";
    char http_get_server1[] = "/v3/weather/now.json?key=SwwwfskBjB6fHVRon&location=shenzhen&language=en&unit=c";
    char HTTPPARA[200] = "AT+HTTPPARA=\"URL\",\"";
    char a[] = "\"";
    sendCMD_waitResp("AT+HTTPINIT", "OK", 2000);
    sendCMD_waitResp("AT+HTTPPARA=\"CID\",1", "OK", 2000);
    strcat(HTTPPARA, http_get_server0);
    strcat(HTTPPARA, http_get_server1);
    strcat(HTTPPARA, a);
    sendCMD_waitResp(HTTPPARA, "OK", 2000);
    if (sendCMD_waitResp("AT+HTTPACTION=0", "200", 5000))
    {
        sendCMD_waitResp("AT+HTTPREAD", "OK", 8000);
    }
    else
    {
        consolep("Get HTTP failed, please check and try again\n");
    }
    sendCMD_waitResp("AT+HTTPTERM", "OK", 2000);
}

void http_post()
{
    char http_post_server0[] = "http://pico.wiki";
    char http_post_server1[] = "/post-data.php";
    char http_content_type[] = "application/x-www-form-urlencoded";
    char http_post_tmp[] = "api_key=tPmAT5Ab3j888&value1=26.44&value2=57.16&value3=1002.95";
    char URL[100] = "AT+HTTPPARA=\"URL\",\"";
    char CONTENT[200] = "AT+HTTPPARA=\"CONTENT\",\"";
    char a[] = "\"";
    sendCMD_waitResp("AT+HTTPINIT", "OK", 2000);
    sendCMD_waitResp("AT+HTTPPARA=\"CID\",1", "OK", 2000);
    strcat(URL, http_post_server0);
    strcat(URL, http_post_server1);
    strcat(URL, a);
    sendCMD_waitResp(URL, "OK", 2000);
    strcat(CONTENT, http_content_type);
    strcat(CONTENT, a);
    sendCMD_waitResp(CONTENT, "OK", 2000);
    if (sendCMD_waitResp("AT+HTTPDATA=62,8000", "DOWNLOAD", 3000))
    {
        if (sendCMD_waitResp(http_post_tmp, "OK", 5000))
        {
            consolep("UART data is read!\n");
        }
        if (sendCMD_waitResp("AT+HTTPACTION=1", "200", 8000))
        {
            consolep("POST successfully!\n");
        }
        else
        {
            consolep("POST failed\n");
        }
        sendCMD_waitResp("AT+HTTPTERM", "OK", 2000);
    }
    else
        consolep("HTTP Post failed,please try again!\n");
}
