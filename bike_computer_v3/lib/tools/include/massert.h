#ifndef MASSERT_H
#define MASSERT_H

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>

#define massert(check, format, ...) \
    do { \
        if(!check) \
        { \
            printf(format, ##__VA_ARGS__); \
            fflush(stdout); \
            assert(check); \
        } \
    } while(0)

#endif