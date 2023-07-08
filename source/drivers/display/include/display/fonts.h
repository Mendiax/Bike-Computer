#ifndef __FONTS_H__
#define __FONTS_H__

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>


#define MAX_HEIGHT_FONT         41
#define MAX_WIDTH_FONT          32
#define OFFSET_BITMAP


#ifdef __cplusplus
 extern "C" {
#endif

typedef struct sFONT
{    
  const uint8_t *table;
  uint16_t width;
  uint16_t height;
  
} sFONT;



/*17, Width */
/*24, Height */
extern const sFONT Font24;

/*14, Width */
/*20, Height */
extern const sFONT Font20;

/*14, Width */
/*20, Height */
extern const  sFONT Font16;

/* 7  Width */
/* 12 Height */
extern const sFONT Font12;

/*5, Width */
/*8, Height */
extern const sFONT Font8;

void getFontSize(uint16_t width, uint16_t height, const sFONT** font, uint8_t* scale);
void getFontSizePreferBiggerFonts(uint16_t width, uint16_t height, const sFONT** font, uint8_t* scale);


#ifdef __cplusplus
}
#endif

#endif