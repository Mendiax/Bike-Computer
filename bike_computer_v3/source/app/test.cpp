// SD
#include <stdio.h>
#include "pico/stdlib.h"
#include "sd_card.h"
#include "ff.h"



#include <string.h>
//#include <stdarg.h>
#include <inttypes.h>
#include <iostream>

#include <pico/stdlib.h>
#include <pico/multicore.h>
#include "hardware/uart.h"


#include "display/print.h"
//  #include "console/console.h"
// #include "display/driver.hpp"


#include "speedometer/speedometer.hpp"
#include "sim868/interface.hpp"
#include "traces.h"

#include "test.hpp"



//#include "IMU.h"
#include "i2c.h"

// #define printf(...) printf(__VA_ARGS__)
// #define printf(...) printf(__VA_ARGS__)


void test_SD()
{
    FRESULT fr;
    FATFS fs;
    FIL fil;
    int ret;
    char buf[100];
    char filename[] = "test02.txt";
START_SD:
    // Wait for user to press 'enter' to continue
    printf("\r\nSD card test. Press 'enter' to start.\r\n");
    while (true)
    {
        buf[0] = getchar();
        if ((buf[0] == '\r') || (buf[0] == '\n'))
        {
            break;
        }
    }
    // Initialize SD card
    if (!sd_init_driver())
    {
        printf("ERROR: Could not initialize SD card\r\n");
        goto START_SD;
    }
    // Mount drive
    fr = f_mount(&fs, "0:", 1);
    if (fr != FR_OK)
    {
        printf("ERROR: Could not mount filesystem (%d)\r\n", fr);
        goto START_SD;
    }
    // Open file for writing ()
    fr = f_open(&fil, filename, FA_WRITE | FA_CREATE_ALWAYS);
    if (fr != FR_OK)
    {
        printf("ERROR: Could not open file (%d)\r\n", fr);
        goto START_SD;
    }
    // Write something to file
    ret = f_printf(&fil, "This is another test\r\n");
    if (ret < 0)
    {
        printf("ERROR: Could not write to file (%d)\r\n", ret);
        f_close(&fil);
        goto START_SD;
    }
    ret = f_printf(&fil, "of writing to an SD card.\r\n");
    if (ret < 0)
    {
        printf("ERROR: Could not write to file (%d)\r\n", ret);
        f_close(&fil);
        goto START_SD;
    }
    // Close file
    fr = f_close(&fil);
    if (fr != FR_OK)
    {
        printf("ERROR: Could not close file (%d)\r\n", fr);
        goto START_SD;
    }
    // Open file for reading
    fr = f_open(&fil, filename, FA_READ);
    if (fr != FR_OK)
    {
        printf("ERROR: Could not open file (%d)\r\n", fr);
        goto START_SD;
    }
    // Print every line in file over serial
    printf("Reading from file '%s':\r\n", filename);
    printf("---\r\n");
    while (f_gets(buf, sizeof(buf), &fil))
    {
        printf(buf);
    }
    printf("\r\n---\r\n");
    // Close file
    fr = f_close(&fil);
    if (fr != FR_OK)
    {
        printf("ERROR: Could not close file (%d)\r\n", fr);
        goto START_SD;
    }
    // Unmount drive
    f_unmount("0:");
    // Loop forever doing nothing
    printf("test_SD finished\r\n");
}

void test_DOF()
{
    //init_i2c();
    I2C_Init();
    // scan();
    // bmp_test();
    printf("I2C init\n");
    IMU_Init();
    printf("IMU init\n");
    float data[3];

    while (1)
    {
        // int16_t magn[3], accel[3], gyro[3];
        //IMU_GetYawPitchRoll(data);
        //sleep_ms(1000);
        Vector3 accel, mag, gyro;
        mpu9250::read_accel(accel);
        mpu9250::read_mag(mag);
        mpu9250::read_gyro(gyro);

        // accel.normalize();
        // mag.normalize();
        // gyro.normalize();

        auto [temp, press] = bmp280::get_temp_press();

        //printf(" accel " FORMAT_INT16_ARR "\t", INS_ARR(accel.arr));
        printf(" magn "  FORMAT_INT16_ARR "\t", INS_ARR(mag.arr));
        //printf(" gyro "  FORMAT_INT16_ARR "\t", INS_ARR(gyro.arr));
        //printf(" temp %" PRId32 "\tpress %" PRId32, temp, press);

        //printf("%" PRIi16 "\t%" PRIi16 "\t%" PRIi16, INS_ARR(gyro.arr));  // angle accel ???
        printf("\n");
        sleep_ms(500000);


        //printf("YawPitchRoll " FORMAT_FLOAT_ARR " gyro " FORMAT_INT16_ARR "\n", INS_ARR(data), INS_ARR(gyro));

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
    // Paint_Println(0, DISPLAY_HEIGHT - font->height, "========================================", font, FONT_FOREGROUND);

    display::display();
}

void test_draw_line()
{
    display::init();
    display::clear();
    display::display();

    printf("driver inited\n");

    // Paint_DrawLine(0,0,4,100);
    // Paint_DrawLine(10,10,300,10);
    // Paint_DrawLine(12,1,120,200);
    // Paint_DrawLine(10,1,30,200);

    Paint_DrawLine(0,0,319,239);
    // Paint_DrawLine(10,10,30,1);
    // Paint_DrawLine(12,1,12,20);
    // Paint_DrawLine(10,1,3,20);

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
        // printf("%s",buffer);
        //        =============================
        printf("==================================\n");
        sleep_ms(200);
    }
}

void test_gps()
{
  consoleLogInit();
  //GPS_test();
}

void test_atInternet()
{
    // DEV_GSM_Module_Init();
    // powerOn;
    // led_blink();
    // DEV_GSM_Delay_ms(5000);
    // check_start();
    // /*
    // AT+CSQ      [ To measure the signal quality ]
    // AT+CREG=1   [ To enable network registration ]
    // AT+CREG?    [ To return the current state of the network registration : Attach/Detach ]
    // AT+COPS=?   [ To return the list of available operators ]
    // AT+COPS=    [ To manually select an available operator ]
    // AT+COPS?    [ To check the current network ]
    // AT+CGATT=1  [ to attach the terminal to GPRS service ]
    // AT+CGATT?   [ To return the current state of GPRS service : Attach/Detach ]
    // AT+CGDCONT=1,"IP","em"    [ To define PDP Context ]
    // AT+CSQ      [ to measure signal quality ]
    // AT+CGACT=1  [ To activate a PDP context ]
    // AT+CGDCONT? [ To return the current state of the PDP context]
    // AT+CGACT=0  [ To deactivate a PDP context ]
    // */
    // sendCMD_waitResp("AT+CSQ?","OK",2000);
    // sendCMD_waitResp("AT+CSQ","OK",2000);
    // sendCMD_waitResp("AT+CREG=1","OK",2000);
    // sendCMD_waitResp("AT+CREG?","OK",2000);
    // sendCMD_waitResp("AT+COPS=?","OK",2000);


}
void test_sim868_interface(void)
{
    sim868::boot();
    sim868::sendRequestLong("AT+CGNSPWR=1",2000);
    for (int i = 1; i < 10;)
    {
        //sim868::sendRequestGetGpsPos();
        sim868::sendRequestLong("AT+CGNSINF", 2000);
        sleep_ms(1000);
    }
    sim868::sendRequestLong("AT+CGNSPWR=0",2000);
    sim868::turnOff();
}

void test_speedData()
{
    // setup
    speed_emulate(20);
    // do updates in loop
    // check avg speed

    // after stop
    // check current speed
    // check distance
}

int main(void)
{
    asm volatile("nop");
    // Initialize chosen serial port
    stdio_init_all();
    //time_init();
    while (!stdio_usb_connected())
    {
        sleep_ms(100);
    }
    tracesSetup();

    // IMU_Init();
    // scan();
    // // test magnetometr
    // #define MAG_ADDR 0x1e
    // #define Register_A    0              // Address of Configuration register A
    // #define Register_B    0x01           // Address of configuration register B
    // #define Register_mode 0x02           // Address of mode register

    // #define X_axis_H   0x03              // Address of X-axis MSB data register
    // #define Z_axis_H   0x05              // Address of Z-axis MSB data register
    // #define Y_axis_H   0x07              // Address of Y-axis MSB data register

    // #define Reg_id 0x0a
    // {
    //     // auto chip_id = I2C_ReadOneByte(MAG_ADDR, 0x0d); // read drdy
    //     // std::cout << "chip_id " << (int)chip_id << std::endl;

    //     // char id[4] = {0};
    //     // I2C_ReadBuff(MAG_ADDR, Reg_id, 3, (uint8_t*)id);
    //     // std::cout << id << std::endl; // H43

    //     // init
    //     // I2C_WriteOneByte(MAG_ADDR, Register_A, 0x70); // 0 11 100 00
    //     // I2C_WriteOneByte(MAG_ADDR, Register_B, 0xa0); // 101 00000    ± 4.7 Ga range
    //     // I2C_WriteOneByte(MAG_ADDR, Register_mode, 0); // 0 00000 00   Continuous-Measurement Mode.
    //     I2C_WriteOneByte(MAG_ADDR, Register_A, 0b00000010); // 0 11 100 00
    //     I2C_WriteOneByte(MAG_ADDR, Register_B, 0x00); // 101 00000    ± 4.7 Ga range
    //     I2C_WriteOneByte(MAG_ADDR, Register_mode, 0); // 0 00000 00   Continuous-Measurement Mode.

    //     sleep_ms(6); // wait a bit


    //     auto read_reg = [](uint8_t addr) -> int16_t
    //     {
    //         uint8_t h = I2C_ReadOneByte(MAG_ADDR, addr);
    //         uint8_t l = I2C_ReadOneByte(MAG_ADDR, addr + 1);

    //         uint16_t value_raw = ((uint16_t)h << 8 | (uint16_t)l);
    //         int16_t value = value_raw > 32768 ? value_raw - 65536 : value_raw;
    //         return value;
    //     };

    //     // I2C_WriteOneByte(MAG_ADDR, Register_mode, 0x01); // 0 00000 00   Continuous-Measurement Mode.
    //     while(1)
    //     {
    //         auto drdy = I2C_ReadOneByte(MAG_ADDR, 0x09); // read drdy
    //         std::cout << "drdy " << (int)drdy << std::endl;
    //         if(drdy & 1)
    //         {
    //             //uint8_t buff[6];
    //             //I2C_ReadBuff(MAG_ADDR, 0x03, 6, buff);
    //             std::cout << "  X " << read_reg(X_axis_H);
    //             std::cout << "  Z " << read_reg(Z_axis_H);
    //             std::cout << "  Y " << read_reg(Y_axis_H) << std::endl;
    //         }

    //         sleep_ms(1000);
    //     }
    // }
    run_tests();
    //test_DOF();


    //test_SD();

    /*
    display test
    */
    printf("[TEST] starting test_driver()\n");
    test_driver();
    printf("[TEST] ending test_driver()\n");
    sleep_ms(1000);
    printf("[TEST] starting test_print()\n");
    test_print();
    printf("[TEST] ending test_print()\n");
    sleep_ms(1000);
    printf("[TEST] starting test_draw_line()\n");
    test_draw_line();
    printf("[TEST] ending test_draw_line()\n");

    sleep_ms(1000);


    /*
    console test
    */
    //test_console();
    //consoleLogInit();

    //init_i2c();
    //bmp_test();
    //test_DOF();

    test_sim868_interface();


    while (1)
    {
        sleep_ms(1000);
    }

    return 0;
}
