#ifndef __SCREEN_FUNC_LABEL_H__
#define __SCREEN_FUNC_LABEL_H__

#include "../frame.h"
#include <stdio.h>
#include "../../../../tools/include/RingBuffer.h"
#include "display/fonts.h"
#include "common.h"

/**
 * @brief create label that fits into frame on pre allocated memory 
 * 
 * @param frame 
 * @param string 
 * @param commonLength if we want to have to labels with the same length we need to get max size of thos string
 * @return LabelSettings* 
 */
void labelSettingsNew(LabelSettings* settings, const Frame& frame, const char* string, size_t commonLength);

void labelSettingsNew(LabelSettings* settings, const Frame& frame, const char* string);

uint16_t labelSettingsGetWidth(LabelSettings* settings);

uint16_t labelSettingsGetHeight(LabelSettings* settings);

void labelSettingsAlign(LabelSettings* settings, const Frame& frame, Align align);

void labelSettingsCreateAndAdd(const Frame& frame, const char* string);

/*prints string*/
void LabelDraw(void *settings);

#endif
