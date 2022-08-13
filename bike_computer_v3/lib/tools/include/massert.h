#ifndef MASSERT_H
#define MASSERT_H

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>

#define massert(check, format, ...) \
    do { \
        if(!(check)) \
        { \
            fprintf(stderr,"\x1b[1;31m" format "\x1b[0m", ##__VA_ARGS__); \
            assert(check); \
        } \
    } while(0)

#endif