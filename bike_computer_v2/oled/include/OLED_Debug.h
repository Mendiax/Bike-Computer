/*****************************************************************************
* | File      	:	DEBUG_OLED.h
* | Author      : Waveshare team
* | Function    :	DEBUG_OLED with printf
* | Info        :
*----------------
* |	This version:   V1.0
* | Date        :   2020-06-16
* | Info        :   Basic version
*
******************************************************************************/
#ifndef __DEBUG_OLED_H
#define __DEBUG_OLED_H

#include "stdio.h"
#include <OLED_Driver.h>
#include <stdlib.h>

#if 1
	#define DEBUG_OLED(__info,...) printf("[DEBUG_OLED] : " __info,##__VA_ARGS__)
#else
	#define DEBUG_OLED(__info,...)  
#endif

#ifdef __cplusplus
 extern "C" {
#endif


#if 1
	#define DEBUG_OLED_ASSERT(__assert__,__info,...) \
		do{\
        if(!__assert__){\
            printf("[DEBUG_OLED_ASSERT]" __FILE__ ":%d:" __info, __LINE__,  ##__VA_ARGS__);\
            DisplayBuffer __display(128, 128);\
            OLED_1in5_Clear();\
            char __errorMsg[256] = {0};\
            snprintf(__errorMsg, 256, "[FATAL_ERROR]:" __FILE__ ":%d:" __info, __LINE__, ##__VA_ARGS__);\
            Paint_Println_multiline(&__display, 0, 0, __errorMsg, &Font8, 0x0f, 0x00);\
            __display.write();\
            do{sleep_ms(1000);} while (1); \
        }\
    }while(0)
#else
	#define DEBUG_OLED_ASSERT(__assert__,__info,...)  
#endif

#ifdef __cplusplus
}
#endif

#endif