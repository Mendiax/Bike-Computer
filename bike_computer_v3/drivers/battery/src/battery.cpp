
#include "battery/battery.h"
#include <hardware/adc.h>

#define FULL_BATTERY 4.2
#define EMPTY_BATTERY 3.0

// factor to  convert adc read to volatge
// 12 bit value -> 0V - 3.3V range
#define CONVERSION_FACTOR (3.3 /((double)(1 << 12) - 1.0))

static float getBatteryVoltage();

void batterySetup(void)
{
    adc_gpio_init(LIPO_GPIO);
}

int getBatteryLevel()
{
    float voltage = getBatteryVoltage();
    int percentage = 100.0 * ((voltage - EMPTY_BATTERY) / (FULL_BATTERY - EMPTY_BATTERY));
    return percentage;
}

static float getBatteryVoltage()
{
    if( adc_get_selected_input() != LIPO_ADC)
        adc_select_input(LIPO_ADC);
    float voltage = (double)(adc_read() * 2) * CONVERSION_FACTOR; // voltage devider 
    return voltage;
}