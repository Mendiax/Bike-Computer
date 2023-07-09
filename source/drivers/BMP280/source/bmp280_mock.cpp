#include <cstdint>
#ifndef BUILD_BMP280_RAW
#include "bmp280_raw.hpp"
/*
simulate bmp280 behaviour
*/

// simple random functions
typedef uint32_t u4;
typedef struct ranctx { u4 a; u4 b; u4 c; u4 d; } ranctx;

#define rot32(x,k) (((x)<<(k))|((x)>>(32-(k))))
u4 ranval( ranctx *x ) {
    u4 e = x->a - rot32(x->b, 27);
    x->a = x->b ^ rot32(x->c, 17);
    x->b = x->c + x->d;
    x->c = x->d + e;
    x->d = e + x->a;
    return x->d;
}

void raninit( ranctx *x, u4 seed ) {
    u4 i;
    x->a = 0xf1ea5eed, x->b = x->c = x->d = seed;
    for (i=0; i<20; ++i) {
        (void)ranval(x);
    }
}

static ranctx random_ctx;

void bmp280_init() {
    raninit(&random_ctx, 0);
}

#define TARGET_TEMP 22.0
#define TARGET_PRESS 1000.0

static float temperature = TARGET_TEMP;
static float pressure = TARGET_PRESS;

void bmp280_update_data()
{

    float random_movement = 0.5 * ((float)ranval(&random_ctx)/(float)UINT32_MAX);
    temperature += random_movement * (temperature > TARGET_TEMP ? -1 : 1);

    random_movement = ((float)ranval(&random_ctx)/(float)UINT32_MAX);
    pressure += random_movement * (pressure > TARGET_PRESS ? -1 : 1);
}

float bmp280_get_temp()
{
    return temperature;

}

float bmp280_get_press()
{
    return pressure;
}

#endif