/*****************************************************************************
* | File      	:   AT_Test.c
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
#include"AT_Test.h"
#include "console/console.h"
char APN[]= "CMNET";

/**power on/off the module**/

/***********AT TEST*************/

/*****Module startup detection**/

void check_start()
{
    while(1){
        sendCMD_waitResp("ATE1","OK",2000);
        DEV_GSM_Delay_ms(2000);
        if(sendCMD_waitResp("AT","OK",2000) == 1){
            consolep("SIM868 is ready\r\n");
            break;
        }
        else{
            module_power();
            consolep("SIM868 is starting up, please wait...\r\n");
            DEV_GSM_Delay_ms(2000);
        }
    }
}

/**Check the network status**/

void check_network()
{
    char CSTT[] = "AT+CSTT=\"" ;
    char a[] = "\"";
    for(int i=1;i<3;i++){
        if(sendCMD_waitResp("AT+CGREG?","0,1",2000) == 1){
            consolep("SIM868 is online\r\n");
            break;
        }
        else{
            consolep("SIM868 is offline, please wait...\r\n");
            DEV_GSM_Delay_ms(2000);
            continue;
        }
    }
    sendCMD_waitResp("AT+CPIN?","OK",2000);
    sendCMD_waitResp("AT+CSQ","OK",2000);
    sendCMD_waitResp("AT+COPS?","OK",2000);
    sendCMD_waitResp("AT+CGATT?","OK",2000);
    sendCMD_waitResp("AT+CGDCONT?","OK",2000);
    sendCMD_waitResp("AT+CSTT?","OK",2000);
    strcat(CSTT,APN);
    strcat(CSTT,a);
    sendCMD_waitResp(CSTT,"OK",2000);
    sendCMD_waitResp("AT+CIICR","OK",2000);
    sendCMD_waitResp("AT+CIFSR","OK",2000);

}

void bearer_config()
{
    char SAPBR[] = "AT+SAPBR=3,1,\"APN\",\"";
    char a[] = "\"";
    sendCMD_waitResp("AT+SAPBR=3,1,\"Contype\",\"GPRS\"", "OK",200);
    strcat(SAPBR,APN);
    strcat(SAPBR,a);
    sendCMD_waitResp(SAPBR, "OK",2000);
    sendCMD_waitResp("AT+SAPBR=1,1", "OK",2000);
    sendCMD_waitResp("AT+SAPBR=2,1", "OK",2000);
//  sendCMD_waitResp("AT+SAPBR=0,1", "OK",2000);
}
