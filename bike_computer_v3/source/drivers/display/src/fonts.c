#include "display/fonts.h"
#include "massert.hpp"
#include "inttypes.h"

// prefering delta
void getFontSize(uint16_t width, uint16_t height, const sFONT** font, uint8_t* scale)
{
  /*5  Width */
  /*8 Height */
  assert(width >= 5 || height >= 8);
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


    uint8_t newScale = 1;
    while (fontWidth <= width && fontHeight <= height)
    {
      newScale += 1;
      fontWidth = fonts[i]->width * newScale;
      fontHeight = fonts[i]->height * newScale;
    }
    newScale -= 1;
    fontWidth = fonts[i]->width * newScale;
    fontHeight = fonts[i]->height * newScale;
    uint16_t newSpace = (width - fontWidth) + (height - fontHeight);
    if(newSpace <= spaceLeft)
    {
      spaceLeft = newSpace;
      *font = fonts[i];
      *scale = newScale;
    }
  }
  massert((*font)->width * *scale <= width && (*font)->height * *scale <= height,
          "fw=%" PRIu16 "> w=%" PRIu16 " or fh=%" PRIu16 "> h=%" PRIu16 "\n",
          (*font)->width * *scale, width, (*font)->height * *scale, height);
}

void getFontSizePreferBiggerFonts(uint16_t width, uint16_t height, const sFONT** font, uint8_t* scale)
{
  /*5  Width */
  /*8 Height */
  if(width < 5 || height < 8)
  {
    return;
  }
  const sFONT* fonts[] = {
    &Font8,
    &Font12,
    &Font16,
    &Font20,
    &Font24
  };
  uint16_t spaceLeft = UINT16_MAX;
  *scale = 1;
  size_t font_id = 4;
  *font = fonts[font_id];
  while ((*font)->width > width && (*font)->height > height)
  {
    font_id--;
    *font = fonts[font_id];
  }
  uint16_t fontWidth = (*font)->width;
  uint16_t fontHeight = (*font)->height;
  *scale = 1;
  while (fontWidth <= width && fontHeight <= height)
  {
    *scale += 1;
    fontWidth = (*font)->width * *scale;
    fontHeight = (*font)->height * *scale;
  }
  *scale -= 1;
}