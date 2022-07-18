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

static void my_assert(bool val)
{
  if(!val)
  {
    while(1)
    {
      printf("[ERRO] fonts\n");
    }
  }
}

static inline void getFontSize(uint16_t width, uint16_t height, const sFONT** font, uint8_t* scale)
{
  /*5  Width */
  /*8 Height */
  my_assert(width >= 5 && height >= 8);
  const sFONT* fonts[] = {
    &Font8,
    &Font12,
    &Font16,
    &Font20,
    &Font24
  };
  uint16_t spaceLeft = UINT16_MAX;
  *scale = 1;
  *font = fonts[0];
  for(size_t i = 0; i < sizeof(fonts)/sizeof(*fonts); i++)
  {
    uint16_t fontWidth = fonts[i]->width;
    uint16_t fontHeight = fonts[i]->height;
    if (fontWidth > width && fontHeight > height)
      continue;
    uint newScale = 1;
    while (fontWidth <= width && fontHeight <= height)
    {
      fontWidth = fonts[i]->width * newScale;
      fontHeight = fonts[i]->height * newScale;
      newScale += 1;
    }
    newScale -= 1;
    fontWidth = fonts[i]->width * newScale;
    fontHeight = fonts[i]->height * newScale;
    uint16_t newSpace = (width - fontWidth) + (height - fontHeight);
    if(newSpace < spaceLeft)
    {
      spaceLeft = newSpace;
      *font = fonts[i];
      *scale = newScale;
    }
  }
  my_assert((*font)->width * *scale <= width && (*font)->height * *scale <= height);
}



#ifdef __cplusplus
}
#endif

#endif