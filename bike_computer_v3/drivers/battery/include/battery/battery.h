#ifndef _BUTTONS_H
#define _BUTTONS_H


#define LIPO_GPIO 27
#define LIPO_ADC 1

void batterySetup(void);

// return battery level in %
int getBatteryLevel();


#endif
