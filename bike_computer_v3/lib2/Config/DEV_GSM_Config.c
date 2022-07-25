/*****************************************************************************
* | File      	:   DEV_GSM_Config.c
* | Author      :   Waveshare team
* | Function    :   Hardware underlying interface
* | Info        :
*----------------
* |	This version:   V3.0
* | Date        :   2019-07-31
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
#include "DEV_GSM_Config.h"
#include "console/console.h"

/**
 * GPIO
 **/
int pwr_en;
int led_en;

uint slice_num;
/**
 * GPIO read and write
 **/
void DEV_GSM_Digital_Write(UWORD Pin, UBYTE Value)
{
    gpio_put(Pin, Value);
}

UBYTE DEV_GSM_Digital_Read(UWORD Pin)
{
    return gpio_get(Pin);
}

/**
 * GPIO Mode
 **/
void DEV_GSM_GPIO_Mode(UWORD Pin, UWORD Mode)
{
    gpio_init(Pin);
    if (Mode == 0 || Mode == GPIO_IN)
    {
        gpio_set_dir(Pin, GPIO_IN);
    }
    else
    {
        gpio_set_dir(Pin, GPIO_OUT);
    }
}

/**
 * delay x ms
 **/
void DEV_GSM_Delay_ms(UDOUBLE xms)
{
    sleep_ms(xms);
}

void DEV_GSM_Delay_us(UDOUBLE xus)
{
    sleep_us(xus);
}

/****************** LED **************/
void led_blink()
{
    DEV_GSM_Digital_Write(led_en, 1);
    DEV_GSM_Delay_ms(500);
    DEV_GSM_Digital_Write(led_en, 0);
    DEV_GSM_Delay_ms(500);
    DEV_GSM_Digital_Write(led_en, 1);
    DEV_GSM_Delay_ms(500);
    DEV_GSM_Digital_Write(led_en, 0);
}
/**********GPIO**************/
void DEV_GSM_GPIO_Init(void)
{
    DEV_GSM_GPIO_Mode(pwr_en, 1);
    DEV_GSM_GPIO_Mode(led_en, 1);

    DEV_GSM_Digital_Write(pwr_en, 1);
    DEV_GSM_Digital_Write(led_en, 0);

    int pin_DTR = 17;
    DEV_GSM_GPIO_Mode(pin_DTR, 1);
    DEV_GSM_Digital_Write(pin_DTR, 1);
}

void module_power()
{
    powerOn;
    DEV_GSM_Delay_ms(2000);
    powerDown;
}
/******************************************************************************
function:	Module Initialize, the library and initialize the pins, SPI protocol
parameter:
Info:
******************************************************************************/
UBYTE DEV_GSM_Module_Init(void)
{
    stdio_init_all();

    // GPIO PIN
    pwr_en = 14;
    led_en = 25;
    DEV_GSM_GPIO_Init();
    uart_init(UART_ID0, BAUD_RATE);
    gpio_set_function(UART_TX_PIN0, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN0, GPIO_FUNC_UART);
    adc_init();
    adc_gpio_init(26);
    adc_set_temp_sensor_enabled(true);
    consolep("DEV_GSM_Module_Init OK \r\n");
    return 0;
}

void Hexstr_To_str(const char *source, unsigned char *dest, int sourceLen)
{
    short i;
    unsigned char highByte, lowByte;
    for (i = 0; i < sourceLen; i += 2)
    {
        highByte = toupper(source[i]);
        lowByte = toupper(source[i + 1]);
        if (highByte > 0x39)
            highByte -= 0x37; //'A'->10 'F'->15
        else
            highByte -= 0x30; //'1'->1 '9'->9
        if (lowByte > 0x39)
            lowByte -= 0x37;
        else
            lowByte -= 0x30;
        dest[i / 2] = (highByte << 4) | lowByte;
    }
}

/*********UART**********/
bool sendCMD_waitResp(const char *str,const char *back,int timeout)
{
    int i = 0;
    uint64_t t = 0;
    char b[500]={0};
    consolef("CMD:%s\r\n", str);
    uart_puts(UART_ID0, str);
    uart_puts(UART_ID0, "\r\n");
    memset(b, 0, sizeof(b));
    t = time_us_64();
    while (time_us_64() - t < timeout * 1000)
    {
        while (uart_is_readable_within_us(UART_ID0, 2000))
        {
            b[i++] = uart_getc(UART_ID0);
            if(i >= 500)
                goto CHECK;
        }
    }
    // debuging no LCD console
    printf("number of bytes read: %d\n", i);
    printf("=====\n");
    for(int j = 0; j < i; j++)
    {
        printf("\'%d\' ", (int)b[j]);
    }
    printf("\n=====\n");


    CHECK:
    b[i] = '\0';
    if (strstr(b, back) == NULL)
    {
        consolef("%s back: %s\r\n", str,b);
        return 0;
    }
    else
    {
        consolef("%s \r\n", b);
        return 1;
    }
    consolep("\r\n");
}


/******************************************************************************
function:	Module exits, closes SPI and BCM2835 library
parameter:
Info:
******************************************************************************/
void DEV_GSM_Module_Exit(void)
{
}
