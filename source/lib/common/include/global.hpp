#pragma once


extern float global_gps_radius; // ~1km


#ifdef BUILD_FOR_HOST
#define GET_GLOBAL(global, local) global
#else
#define GET_GLOBAL(global, local) local
#endif