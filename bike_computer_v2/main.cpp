#include <stdlib.h>
#include <cstdint>
#include <stdio.h>
//#include "pico/stdlib.h"
#include <OLED_DEBUG.h>
#include "hardware/spi.h"
#include <OLED_Driver.h>
#include <OLED_Print.h>
#include <fonts.h>
#include <speedometer.h>
#include <iostream>
#include <bitset>
#include "hardware/adc.h"
#include <cmath>

#include <traces.h>

#define BTN 8
#define LIPO 29
#define CHARGING 24

#include <RingBuffer.h>
// copied
#include <display/display.h>
#include <display/screen.h>
#define REFRESH_RATE_TARGET_HZ 30
#define MIN_DELAY (1000 / REFRESH_RATE_TARGET_HZ)

// uint8_t allocatedMemory_rearShockBuffer[sizeof(uint8_t)*SCREEN_WIDTH + sizeof(RingBuffer)] = {0};
// uint8_t allocatedMemory_speedBuffer[sizeof(uint8_t)*SCREEN_WIDTH + sizeof(RingBuffer)] = {0};

SensorData sensorData = {0}; /*(SensorData){
     .rearShockBuffer = 0;//ring_buffer_create_static(allocatedMemory_rearShockBuffer,sizeof(uint8_t), SCREEN_WIDTH),
     .speedBuffer = 0;//ring_buffer_create_static(allocatedMemory_speedBuffer,sizeof(uint8_t), SCREEN_WIDTH),
     .speedMax = 0.0,
     .speedAvgVal = 0.0,
     .speedAvgCnt = 0,
     .speedDistance = 0,
     .time = 0
     };*/

static void loop();

/*
    setup function
*/
int main()
{
    //traces setup
    tracesSetup();


    // data buffer setup
    sensorData.rearShockBuffer = ring_buffer_create(sizeof(uint8_t), SCREEN_WIDTH);
    sensorData.speedBuffer = ring_buffer_create(sizeof(uint8_t), SCREEN_WIDTH);

    stdio_init_all();
#if 0
        int startX;
        sleep_ms(5000);
        printf("pico ON");
        std::cin >> startX;
#endif

    /*OLED_1in5_Init();
    OLED_1in5_Clear();


    DisplayBuffer display(128, 128);

    Paint_DrawChar(&display, 0,0, 'x', &Font8, 0x0f, 0x00);
    Paint_DrawChar(&display, 5,0, 'd', &Font8, 0x0f, 0x00);
    Paint_DrawChar(&display, 10,0, 'd', &Font8, 0x0f, 0x00);

    Paint_Println(&display, 0,10, "hmmm", &Font8, 0x0f, 0x00);

    DEBUG_OLED_ASSERT(false, "testowanie Assert");

    display.write();



    // Whell sensor test

    //printf("Hello GPIO IRQ\n");
    //gpio_set_irq_enabled_with_callback(9, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    //gpio_set_irq_enabled_with_callback(PIN_SPEED, GPIO_IRQ_EDGE_RISE, true, &speed_update);

*/
    
    //turn of power led
    gpio_init(25); //power led
    gpio_set_dir(25, GPIO_OUT);
    gpio_put(25, 0);

    // ui btn setup
    gpio_init(BTN);
    gpio_set_dir(BTN, GPIO_IN);
    gpio_pull_up(BTN);

    // battery setup
    adc_init();
    adc_gpio_init(LIPO);
    adc_gpio_init(26); //temp

    

    // adc_gpio_init(26);
    // adc_gpio_init(27);
    // adc_gpio_init(28);
    // adc_gpio_init(29);

    // adc_select_input(0);
    // float conversion_factor = 3.0 * 3.3f / (1 << 12);
    // uint16_t voltage;// = adc_read();
    // while (1)
    // {
    //     adc_select_input(0);
    //     voltage = adc_read();
    //     printf("U0=%5u", voltage);
    //     adc_select_input(1);
    //     voltage = adc_read();
    //     printf("U1=%5u", voltage);
    //     adc_select_input(2);
    //     voltage = adc_read();
    //     printf("U2=%5u", voltage);
    //     adc_select_input(3);
    //     voltage = adc_read();
    //     printf("U3=%5u\n", voltage);
    //     sleep_ms(100);
    // }
    

    gpio_init(CHARGING);
    gpio_set_dir(CHARGING, GPIO_IN);

    // sensorData.speedAvgCnt = 0.0;
    // sensorData.speedAvgVal = 0.0;
    // sensorData.speedMax = 0.0;

    speed_new();

    Display_init(&sensorData);
    OLED_1in5_Clear();

    while (true)
    {
        loop();
    }
}
static bool isBtnPressed(uint gpio);

static int getBatteryLevel();
static float getTemp();

#define SIM_INPUT 0
#define PRINT_FRAME_TIME 0
#define PRINT_LIPO_DATA 1

void loop()
{
    //static int test;
    //test++;
    //TRACE_DEBUG(1, mainE, "test trace %d\n", test);
    sensorData.lipoCharging = gpio_get(CHARGING);
    sensorData.lipoLevel = getBatteryLevel();

    if (isBtnPressed(BTN))
        Display_incDisplayType();

    static float velocity;
// get data for speed and distance
#if SIM_INPUT
    velocity += 1;
    velocity = velocity > 30.0 ? 0.0 : velocity;
    sensorData.speedDistance += 10;
    sensorData.speedDistanceHundreth += velocity;
    sensorData.speedDistanceHundreth = sensorData.speedDistanceHundreth % 100;
#else
    velocity = speed_mps_to_kmph(speed_getSpeed());
    sensorData.speedDistance = speed_getDistance() / 1000;
    sensorData.speedDistanceHundreth = (speed_getDistance() / 100) % 100;
#endif

    // do some calculations
    sensorData.speedMax = std::max(sensorData.speedMax, velocity);
    uint8_t bufferVelocity = (uint8_t)velocity;
    ring_buffer_push_overwrite(sensorData.speedBuffer, (char *)&bufferVelocity);

    absolute_time_t currentTime = get_absolute_time();
    sensorData.time = to_ms_since_boot(currentTime) / 1000;
    sensorData.speedAvgVal = sensorData.speedDistance / ((double)sensorData.time / 3600.0);

    // lower refresh rate to save some energy
    static absolute_time_t lastDisplayUpdate;
    int64_t delta_time_ms = us_to_ms(absolute_time_diff_us(lastDisplayUpdate, currentTime));
    if (delta_time_ms < 100)
    {
        // TODO
        // add sleep time to save energy after creating interrupt for btn
        // int64_t timeLeft = 100 - delta_time_ms;
        return;
    }
#if PRINT_FRAME_TIME
    absolute_time_t updateStart = get_absolute_time();
    Display_update();
    absolute_time_t updateEnd = get_absolute_time();
    int64_t updateTime = us_to_ms(absolute_time_diff_us(updateStart, updateEnd));
    std::string log = "update:" + std::to_string(updateTime) + "ms";

    Paint_Println(&__display, 0, 0, log.c_str(), &Font8, 0x0f, 0x00);
    __display.write();
    sleep_ms(100);
#else
    Display_update();
    getBatteryLevel();
    getTemp();
    
    //std::string log = "btn:" + std::to_string(gpio_get(BTN));
    //Paint_Println(&__display, 0 * Font8.width, 1 * Font8.height, log.c_str(), &Font8, 0x0f, 0x00);
    __display.write();
#endif
    //Screen_draw();// TODO fix battery
}

static bool isBtnPressed(uint gpio)
{
    static absolute_time_t lastPress;
    absolute_time_t currentTime = get_absolute_time();
    int64_t delta_time_ms = us_to_ms(absolute_time_diff_us(lastPress, currentTime));
    if (delta_time_ms < 100)
    {
        return false;
    }
    static bool released;
    released = true; // TODO add this functionality
    lastPress = get_absolute_time();
    bool ret = released ? !gpio_get(BTN) /*negate bc it is pullup*/ : false;

    if(ret)
        TRACE_DEBUG(0, BUTTONS, "isBtnPressed return=%dms\n", ret);
    return ret;
}
static float getTemp()
{
    float conversion_factor = 3.28f / (1 << 12);
    adc_select_input(0);
    float voltage = adc_read() * conversion_factor;

    const float U = 3.28f;
    const float R1 = 3270;
    const float R3 = 1;
    float Rt = (voltage/(U-voltage)) * R1 - R3;

    const float T0 = 298.15;
    const float R0 = 9170;
    const float B = 4000;
    const float T = (B * T0)/\
                    (T0*std::log(Rt/R0) + B);

    const int Tc = T - 273.15;


    std::string log = "temp:" + std::to_string(Tc) + "C";
    Paint_Println(&__display, 16 * Font8.width, 0, log.c_str(), &Font8, 0x0f, 0x00);
    //__display.write();
    return voltage;
}

static int getBatteryLevel()
{
    float conversion_factor = 3.0 * 3.3f / (1 << 12);
    float full_battery = 4.2;
    float empty_battery = 3.0;
    adc_select_input(3); // TODO move it somwere else if  use another adc
    float voltage = adc_read() * conversion_factor;
    int percentage = 100 * ((voltage - empty_battery) / (full_battery - empty_battery));

#if PRINT_LIPO_DATA
    std::string log = "bat:" + std::to_string(percentage) + "\% chrg:" + std::to_string(gpio_get(CHARGING));
    Paint_Println(&__display, 0, 0, log.c_str(), &Font8, 0x0f, 0x00);
    //__display.write();
    //sleep_ms(100);
#endif
    return percentage;
}