#include "Pico-SIM868-Test.h"

int at_test()
{
    DEV_GSM_Module_Init();
    powerOn;
    led_blink();
    DEV_GSM_Delay_ms(5000);
    check_start();
    check_network();
    powerDown;
    return true;
}

int BT_test()
{
    DEV_GSM_Module_Init();
    powerOn;
    led_blink();
    DEV_GSM_Delay_ms(5000);
    check_start();
    bluetooth_scan();
    powerDown;
    return true;
}

int GPS_test()
{
    DEV_GSM_Module_Init();
    powerOn;
    led_blink();
    DEV_GSM_Delay_ms(5000);
    check_start();
    while (1)
    {
        get_gps_info();
    }
    powerDown;
    return true;
}
int HTTP_test()
{
    DEV_GSM_Module_Init();
    powerOn;
    led_blink();
    DEV_GSM_Delay_ms(5000);
    check_start();
    check_network();
    bearer_config();
    while (1)
    {
        http_get();
        http_post();
    }
    powerDown;
    return true;
}

int phone_call_test()
{
    DEV_GSM_Module_Init();
    powerOn;
    led_blink();
    DEV_GSM_Delay_ms(5000);
    check_start();
    check_network();
    phone_call("10086" , 15);
    powerDown;
    return true;
}

int SMS_test(){
    char sms_info[] = "Hello, I'm Pico-SIM868!";
    DEV_GSM_Module_Init();
    powerOn;
    led_blink();
    DEV_GSM_Delay_ms(5000);
    check_start();
    check_network();
    phone_call("10086" , 15);
    SMS("10086", sms_info);
    powerDown;
    return true;
}
