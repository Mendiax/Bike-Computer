#ifndef __MODULE_H__
#define __MODULE_H__

#include "ringbuffer.h"

typedef struct Module
{
    RingBuffer* buffer;
    double last_update;    
}Module;

#endif
