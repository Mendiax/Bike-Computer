#ifndef __SCREEN_FUNC_LABEL_H__
#define __SCREEN_FUNC_LABEL_H__

#include "../frame.h"
#include <stdio.h>
#include "../../../../tools/include/RingBuffer.h"
#include "display/fonts.h"

typedef struct LabelData
{
    
} LabelData;

/*print string*/
void LabelDraw(void *data, void *settings, Frame *plotFrame);

#endif
