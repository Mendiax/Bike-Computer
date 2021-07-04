#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include <stdio.h>

#ifdef __cplusplus
extern "C"{
#endif

void consoleLogInit();
void consoleLog(const char* fmt, ...);

#define consolep(fmt) consolef(fmt "%s", "")

#define consolef(fmt, ...) do{              \
    printf(fmt, __VA_ARGS__);               \
    char buffer[65] = {0};                  \
    snprintf(buffer, 64,fmt, __VA_ARGS__);  \
    consoleLog(buffer);                     \
}while(0)

#ifdef __cplusplus
}
#endif

#endif