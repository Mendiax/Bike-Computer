#ifndef __MASSERT_HPP__
#define __MASSERT_HPP__

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>

#ifdef __cplusplus
#include <sstream>
#endif

#define massert(check, format, ...) \
    do { \
        if(!(check)) \
        { \
            fprintf(stderr,"\x1b[1;31m" format "\x1b[0m", ##__VA_ARGS__); \
            assert(check); \
        } \
    } while(0)

#ifdef __cplusplus
#define massert_range(x, bottom_range, top_range) \
    do { \
        if(!((x) >= (bottom_range) && (x) <= (top_range))) \
        { \
            std::stringstream ss; \
            ss << #x "=" << x << " is out of range: [" << bottom_range << "," << top_range << "]\n"; \
            massert(false, "%s", ss.str().c_str()); \
        } \
    } while(0)
#endif

#endif