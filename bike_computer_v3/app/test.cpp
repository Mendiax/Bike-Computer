
#include <stdio.h>
#include <string.h>
//#include <stdarg.h>

#include <pico/stdlib.h>
#include <pico/multicore.h>

#include "hardware/uart.h"

//#define ROTATE
#include "display/print.h"
#include "console/console.h"
#include "display/driver.hpp"

#include "Pico-SIM868-Test.h"

// TODO split tests into files


// SD
//#include <stdio.h>
//#include "pico/stdlib.h"
// #include "sd_card.h"
// #include "ff.h"

#include "IMU.h"
// #include "I2C.h"
#include "BMP280_pico.h"

// void test_SD()
// {
//     FRESULT fr;
//     FATFS fs;
//     FIL fil;
//     int ret;
//     char buf[100];
//     char filename[] = "test02.txt";

// START_SD:
//     // Wait for user to press 'enter' to continue
//     printf("\r\nSD card test. Press 'enter' to start.\r\n");
//     while (true)
//     {
//         buf[0] = getchar();
//         if ((buf[0] == '\r') || (buf[0] == '\n'))
//         {
//             break;
//         }
//     }

//     // Initialize SD card
//     if (!sd_init_driver())
//     {
//         printf("ERROR: Could not initialize SD card\r\n");
//         goto START_SD;
//     }

//     // Mount drive
//     fr = f_mount(&fs, "0:", 1);
//     if (fr != FR_OK)
//     {
//         printf("ERROR: Could not mount filesystem (%d)\r\n", fr);
//         goto START_SD;
//     }

//     // Open file for writing ()
//     fr = f_open(&fil, filename, FA_WRITE | FA_CREATE_ALWAYS);
//     if (fr != FR_OK)
//     {
//         printf("ERROR: Could not open file (%d)\r\n", fr);
//         goto START_SD;
//     }

//     // Write something to file
//     ret = f_printf(&fil, "This is another test\r\n");
//     if (ret < 0)
//     {
//         printf("ERROR: Could not write to file (%d)\r\n", ret);
//         f_close(&fil);
//         goto START_SD;
//     }
//     ret = f_printf(&fil, "of writing to an SD card.\r\n");
//     if (ret < 0)
//     {
//         printf("ERROR: Could not write to file (%d)\r\n", ret);
//         f_close(&fil);
//         goto START_SD;
//     }

//     // Close file
//     fr = f_close(&fil);
//     if (fr != FR_OK)
//     {
//         printf("ERROR: Could not close file (%d)\r\n", fr);
//         goto START_SD;
//     }

//     // Open file for reading
//     fr = f_open(&fil, filename, FA_READ);
//     if (fr != FR_OK)
//     {
//         printf("ERROR: Could not open file (%d)\r\n", fr);
//         goto START_SD;
//     }

//     // Print every line in file over serial
//     printf("Reading from file '%s':\r\n", filename);
//     printf("---\r\n");
//     while (f_gets(buf, sizeof(buf), &fil))
//     {
//         printf(buf);
//     }
//     printf("\r\n---\r\n");

//     // Close file
//     fr = f_close(&fil);
//     if (fr != FR_OK)
//     {
//         printf("ERROR: Could not close file (%d)\r\n", fr);
//         goto START_SD;
//     }

//     // Unmount drive
//     f_unmount("0:");

//     // Loop forever doing nothing
//     while (true)
//     {
//         sleep_ms(1000);
//     }
// }

void test_DOF()
{
    init_i2c();
    scan();
    // bmp_test();
    consolep("I2C init\n");
    IMU_Init();
    consolep("IMU init\n");
    float data[3];
#define INS_ARR(arr) arr[0], arr[1], arr[2]
#define FORMAT_INT16 "% 3d"
#define FORMAT_INT16_ARR "[" FORMAT_INT16 "," FORMAT_INT16 "," FORMAT_INT16 "]"
#define FORMAT_FLOAT "% 3.2f"
#define FORMAT_FLOAT_ARR "[" FORMAT_FLOAT "," FORMAT_FLOAT "," FORMAT_FLOAT "]"
    while (0)
    {
        // int16_t magn[3], accel[3], gyro[3];
        IMU_GetYawPitchRoll(data);
        sleep_ms(1000);
        consolef("YawPitchRoll" FORMAT_FLOAT_ARR " accel" FORMAT_INT16_ARR " magn" FORMAT_INT16_ARR " gyro" FORMAT_INT16_ARR "\n", INS_ARR(data), INS_ARR(accel), INS_ARR(magn), INS_ARR(gyro));
    }
}

void test_driver()
{
    display::init();
    display::clear();
    display::display();

    printf("driver inited\n");

    // line on from top left corner
    for (size_t i = 0; i < DISPLAY_WIDTH / 2; i++)
    {
        display::setPixel(i, {0xf, 0xf, 0xf});
    }
    // full line in second row
    for (size_t i = 0; i < DISPLAY_WIDTH; i++)
    {
        display::setPixel(i + DISPLAY_WIDTH * 2, {0xf, 0xf, 0xf});
    }
    // line in first column
    for (size_t i = 0; i < DISPLAY_HEIGHT; i++)
    {
        display::setPixel(i * DISPLAY_WIDTH, {0xf, 0xf, 0xf});
    }
    display::display();
}

void test_print()
{
    display::init();
    display::clear();
    display::display();

    printf("driver inited\n");

    // rectangle on from top left corner
    for (size_t i = 0; i < DISPLAY_HEIGHT / 4; i++)
    {
        for (size_t j = 0; j < DISPLAY_WIDTH / 4; j++)
        {
            Paint_SetPixel(j, i, {0xf, 0xf, 0xf});
        }
    }

    for (size_t i = DISPLAY_HEIGHT - 10; i < DISPLAY_HEIGHT; i++)
    {
        for (size_t j = DISPLAY_WIDTH - 10; j < DISPLAY_WIDTH; j++)
        {
            Paint_SetPixel(j, i, {0xf, 0xf, 0xf});
        }
    }
    //display::clear();
    const sFONT* font = &Font16;
    printf("\n");
    Paint_DrawChar(100, 0, 'x', font);
    printf("\n");
    Paint_DrawChar(240, 10, 'x', font);

    printf("\n");
    Paint_Println(0, DISPLAY_HEIGHT - font->height, "========================================", font, FONT_FOREGROUND);

    display::display();
}

void test_console()
{
    consoleLogInit();
    for (int i = 0; i < 20; i++)
    {
        // enum
        // {
        //     BUFFER_SIZE = 100
        // };
        // char buffer[BUFFER_SIZE] = {0};
        // snprintf(buffer, BUFFER_SIZE, "_%2d_-/-/10-/-/-/-/20-/-25/-/30", i);
        // consolef("%s",buffer);
        //        =============================
        consolep("==================================\n");
        sleep_ms(200);
    }
}

void test_gps()
{
  consoleLogInit();
  GPS_test();
}

void test_atInternet()
{
    DEV_GSM_Module_Init();
    powerOn;
    led_blink();
    DEV_GSM_Delay_ms(5000);
    check_start();
    /*
    AT+CSQ      [ To measure the signal quality ]
    AT+CREG=1   [ To enable network registration ]
    AT+CREG?    [ To return the current state of the network registration : Attach/Detach ]
    AT+COPS=?   [ To return the list of available operators ]
    AT+COPS=    [ To manually select an available operator ]
    AT+COPS?    [ To check the current network ]
    AT+CGATT=1  [ to attach the terminal to GPRS service ] 
    AT+CGATT?   [ To return the current state of GPRS service : Attach/Detach ]
    AT+CGDCONT=1,"IP","em"    [ To define PDP Context ]
    AT+CSQ      [ to measure signal quality ]  
    AT+CGACT=1  [ To activate a PDP context ]  
    AT+CGDCONT? [ To return the current state of the PDP context]
    AT+CGACT=0  [ To deactivate a PDP context ] 
    */
    sendCMD_waitResp("AT+CSQ?","OK",2000);
    sendCMD_waitResp("AT+CSQ","OK",2000);
    sendCMD_waitResp("AT+CREG=1","OK",2000);
    sendCMD_waitResp("AT+CREG?","OK",2000);
    sendCMD_waitResp("AT+COPS=?","OK",2000);


}

int main(void)
{
    asm volatile("nop");
    // Initialize chosen serial port
    stdio_init_all();
    while (!stdio_usb_connected())
    {
        sleep_ms(100);
    }

    /*
    display test
    */
    // printf("[TEST] starting test_driver()\n");
    // test_driver();
    // printf("[TEST] ending test_driver()\n");
    // sleep_ms(1000);
    // printf("[TEST] starting test_print()\n");
    // test_print();
    // printf("[TEST] ending test_print()\n");

    /*
    console test
    */
    //test_console();
    consoleLogInit();
    //test_atInternet();
//     powerOn;
    //at_test();
    GPS_test();
    test_gps();
    //HTTP_test();





    // int x = 0;
    // gpio_init(25);
    // gpio_set_dir(25, GPIO_OUT);
    // while (x < 2)
    // {
    //     printf("xd %d\n", x);
    //     x += 2;
    //     gpio_put(25,1);
    //     sleep_ms(500);
    //     gpio_put(25,0);
    //     sleep_ms(500);
    // }

    // sleep_ms(1000);
    // consoleLogInit();

    // wait for intput
    // while (true)
    // {
    //     char c = getchar();
    //     if ((c == '\r') || (c == '\n'))
    //     {
    //         break;
    //     }
    // }
    // sleep_ms(1000);

    // test_SD();

    // return 0;
    //  gpio_init(14);
    //  gpio_set_dir(14, GPIO_OUT);
    //  gpio_put(14, 1);

    // at_test();
    // DEV_GSM_Module_Init();
    // powerOn;
    // led_blink();
    //  BT_test();
    // GPS_test();
    //  HTTP_test();
    //  phone_call_test();
    //  SMS_test();
    while (1)
    {
        sleep_ms(1000);
    }

    return 0;
}
