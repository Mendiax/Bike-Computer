#include "traces.h"
#include <pico/sync.h>


uint32_t tracesOn[NO_TRACES] = {0};
mutex_t tracesMutex;

