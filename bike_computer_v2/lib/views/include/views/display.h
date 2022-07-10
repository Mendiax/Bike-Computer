#ifndef __display_H__
#define __display_H__

#include "screen.h"
#include "screenfunc/plot.h"
#include "screenfunc/lastval.h"
#include "window.h"
#include "screenfunc/label.h"
#include "screenfunc/val.h"

#define MAX_NUMBER_OF_WINDOWS 8

#include "view.h"

#define SETTING_SIZE std::max(std::max(std::max(sizeof(LastValSettings), sizeof(PlotSettings)), sizeof(LabelSettings)), sizeof(ValSettings))
uint8_t allocData[SETTING_SIZE * MAX_NUMBER_OF_WINDOWS + sizeof(View)];

void* getSettings(unsigned id){
    assert(id < MAX_NUMBER_OF_WINDOWS);
    return allocData + sizeof(View) + id * SETTING_SIZE;
}

typedef struct SensorData
{
    RingBuffer* rearShockBuffer;
    RingBuffer* speedBuffer;
    float speedMax;
    float speedAvgVal;
    unsigned long speedAvgCnt;
    unsigned speedDistance;
    unsigned speedDistanceHundreth;
    unsigned long time;
    bool lipoCharging;
    int lipoLevel; // in %
} SensorData;

typedef struct Display
{
    const uint8_t *dataAlloc;
    SensorData *data;
    uint8_t currentType;
} Display;

Display _Display;

typedef void (*view_new_func)(void);



/*
    speed, distance 
*/
void view0(void)
{
    uint8_t numberOfWindows = 7;
    View *newView = (View *)_Display.dataAlloc;
    view_new_inAllocatedPlace(newView, numberOfWindows);
    uint8_t settingsId = 0;
    uint8_t windowId = 0;

    /*ValSettings *lipoSettings = (ValSettings *)getSettings(settingsId++);
    *lipoSettings = (ValSettings){
        .format = "%d",
        .maxLength = 6,
        .textSize = &Font20,
        .textScale = 1,
        .offsetX = 0,
        .offsetY = 0};
    Window_new_inPlace(&newView->windows[windowId++],
                       (Frame){0, 0, SCREEN_WIDTH/2, SCREEN_HEIGHT},
                       (void *)&(_Display.data->lipoLevel),
                       (void *)lipoSettings,
                       ValDraw);*/

    LastValSettings *lastValSettings = (LastValSettings *)getSettings(settingsId++);
    *lastValSettings = (LastValSettings){
        .format = "%2d",
        .maxLength = 2,
        .textSize = &Font24,
        .textScale = 3,
        .offsetX = 2,
        .offsetY = 0};
    Window_new_inPlace(&newView->windows[windowId++],
                       (Frame){0, 20, SCREEN_WIDTH/2, SCREEN_HEIGHT/2},
                       (void *)_Display.data->speedBuffer,
                       (void *)lastValSettings,
                       LastValDrawByte);

    Frame kph = {17*3*2 - 2, 10 + 20, 0, 0};
    const sFONT* kphFont = &Font20;

    LabelSettings *headerSettings1 = (LabelSettings *)getSettings(settingsId++);
    *headerSettings1 = (LabelSettings){
        .string = "__",
        .textSize = kphFont,
        .offsetX = 0,
        .offsetY = 1};
    Window_new_inPlace(&newView->windows[windowId++],
                       kph,
                       (void *)0,
                       (void *)headerSettings1,
                       LabelDraw);

    LabelSettings *headerSettings0 = (LabelSettings *)getSettings(settingsId++);
    *headerSettings0 = (LabelSettings){
        .string = "km",
        .textSize = kphFont,
        .offsetX = 0,
        .offsetY = 0};
    Window_new_inPlace(&newView->windows[windowId++],
                       kph,
                       (void *)0,
                       (void *)headerSettings0,
                       LabelDraw);

    LabelSettings *headerSettings2 = (LabelSettings *)getSettings(settingsId++);
    *headerSettings2 = (LabelSettings){
        .string = "h",
        .textSize = kphFont,
        .offsetX = (unsigned)kphFont->width/2,
        .offsetY = (unsigned)kphFont->height + 1};
    Window_new_inPlace(&newView->windows[windowId++],
                       kph,
                       (void *)0,
                       (void *)headerSettings2,
                       LabelDraw);



    
    // --- distance ---
    uint8_t offset = lastValSettings->textSize->height * lastValSettings->textScale + 20;
    ValSettings *valSettings = (ValSettings *)getSettings(settingsId++);
    *valSettings = (ValSettings){
        .format = "%3d",
        .maxLength = 3,
        .textSize = &Font24,
        .textScale = 2,
        .offsetX = 2,
        .offsetY = 0};
    Window_new_inPlace(&newView->windows[windowId++],
                       (Frame){0, offset, SCREEN_WIDTH/2, SCREEN_HEIGHT/2},
                       (void *)&(_Display.data->speedDistance),
                       (void *)valSettings,
                       ValDrawInt);
    
    Frame km = {17*3*2 - 2, offset, 0, 0};
    const sFONT* kmFont = &Font20;

    LastValSettings *valSettings1 = (LastValSettings *)getSettings(settingsId++);
    *valSettings1 = (LastValSettings){
        .format = "%2d",
        .maxLength = 2,
        .textSize = kmFont,
        .textScale = 1,
        .offsetX = 0,
        .offsetY = 0};
    Window_new_inPlace(&newView->windows[windowId++],
                       km,
                       (void *)&(_Display.data->speedDistanceHundreth),
                       (void *)valSettings1,
                       ValDrawInt);

    LabelSettings *headerSettings3 = (LabelSettings *)getSettings(settingsId++);
    *headerSettings3 = (LabelSettings){
        .string = "km",
        .textSize = kmFont,
        .offsetX = 0,
        .offsetY = kmFont->height};
    Window_new_inPlace(&newView->windows[windowId++],
                       km,
                       (void *)0,
                       (void *)headerSettings3,
                       LabelDraw);
    

    DEBUG_OLED_ASSERT(numberOfWindows >= windowId, "Too many windows %u", windowId);
    DEBUG_OLED_ASSERT(numberOfWindows >= settingsId, "Too many settings %u", settingsId);
    DEBUG_OLED_ASSERT(MAX_NUMBER_OF_WINDOWS >= numberOfWindows, 
                      "Increase macro MAX_NUMBER_OF_WINDOWS-> %u", 
                      numberOfWindows);
}


/* 
    max, avg display
*/
void view1(void)
{
    uint8_t numberOfWindows = 4;
    uint8_t settingsId = 0;
    uint8_t windowId = 0;

    View *newView = (View *)_Display.dataAlloc;
    view_new_inAllocatedPlace(newView, numberOfWindows);

    ValSettings *maxSpeedSettings = (ValSettings *)getSettings(settingsId++);
    *maxSpeedSettings = (ValSettings){
        .format = "%2d",
        .maxLength = 6,
        .textSize = &Font24,
        .textScale = 2,
        .offsetX = 0,
        .offsetY = 44};
    Window_new_inPlace(&newView->windows[windowId++],
                       (Frame){0, 0, SCREEN_WIDTH/2, SCREEN_HEIGHT},
                       (void *)&(_Display.data->speedMax),
                       (void *)maxSpeedSettings,
                       ValDraw);
    Window_new_inPlace(&newView->windows[windowId++],
                       (Frame){SCREEN_WIDTH/2, 0, SCREEN_WIDTH/2, SCREEN_HEIGHT},
                       (void *)&(_Display.data->speedAvgVal),
                       (void *)maxSpeedSettings,
                       ValDraw);

    /*top header*/
    LabelSettings *header2Settings = (LabelSettings *)getSettings(settingsId++);
    *header2Settings = (LabelSettings){
        .string = "max   avg",
        .textSize = &Font20,
        .offsetX = 2,
        .offsetY = 20};
    Window_new_inPlace(&newView->windows[windowId++],
                       (Frame){0, 0, SCREEN_WIDTH, SCREEN_HEIGHT},
                       (void *)0,
                       (void *)header2Settings,
                       LabelDraw);

    ValSettings *topBarSettings = (ValSettings *)getSettings(settingsId++);
    *topBarSettings = (ValSettings){
        .format = "%d:%02d:%02d",
        .maxLength = 15,
        .textSize = &Font16,
        .textScale = 2,
        .offsetX = 0,
        .offsetY = 100};
    Window_new_inPlace(&newView->windows[windowId++],
                       (Frame){0, 0, SCREEN_WIDTH, SCREEN_HEIGHT_YELLOW},
                       (void *)&(_Display.data->time),
                       (void *)topBarSettings,
                       ValDrawTime);


    /*BAT LEVEL*/
   /* ValSettings *lipoSettings = (ValSettings *)getSettings(settingsId++);
    *lipoSettings = (ValSettings){
        .format = "%d",
        .maxLength = 6,
        .textSize = &Font20,
        .textScale = 1,
        .offsetX = 0,
        .offsetY = 0};
    Window_new_inPlace(&newView->windows[windowId++],
                       (Frame){0, 0, SCREEN_WIDTH/2, SCREEN_HEIGHT},
                       (void *)&(_Display.data->lipoLevel),
                       (void *)lipoSettings,
                       ValDraw);*/

    DEBUG_OLED_ASSERT(numberOfWindows >= windowId, "Too many windows %u", windowId);
    DEBUG_OLED_ASSERT(numberOfWindows >= settingsId, "Too many settings %u", settingsId);
    DEBUG_OLED_ASSERT(MAX_NUMBER_OF_WINDOWS >= numberOfWindows, 
                      "Increase macro MAX_NUMBER_OF_WINDOWS-> %u", 
                      numberOfWindows);
}


/*
    speed plot
*/
void view2(void)
{
    View *newView = (View *)_Display.dataAlloc;
    view_new_inAllocatedPlace(newView, 1);
    PlotSettings *plotSettings = (PlotSettings *)getSettings(0);
    *plotSettings = {
        .min = 0,
        .max = 30,
        .autoMax = true,
        .autoMin = false,
        .offset = 5};
    Window_new_inPlace(&newView->windows[0],
                       (Frame){0, 0, SCREEN_WIDTH, SCREEN_HEIGHT},
                       (void *)_Display.data->speedBuffer,
                       (void *)plotSettings,
                       PlotDrawByte);
}


/*
    rear shock plot
*/
void view3(void)
{
    View *newView = (View *)_Display.dataAlloc;
    view_new_inAllocatedPlace(newView, 2);
    LastValSettings *topBarSettings = (LastValSettings *)getSettings(0);
    *topBarSettings = (LastValSettings){
        .format = "rear shock: %2d",
        .maxLength = 15,
        .textSize = &Font12,
        .textScale = 1,
        .offsetX = 0,
        .offsetY = 0};
    Window_new_inPlace(&newView->windows[0],
                       (Frame){0, 0, SCREEN_WIDTH, SCREEN_HEIGHT_YELLOW},
                       (void *)_Display.data->rearShockBuffer,
                       (void *)topBarSettings,
                       LastValDrawByte);

    
    PlotSettings *plotSettings = (PlotSettings *)getSettings(1);
    *plotSettings = {
        .min = 0,
        .max = 100,
        .autoMax = true,
        .autoMin = true,
        .offset = 5};
    Window_new_inPlace(&newView->windows[1],
                       (Frame){0, SCREEN_HEIGHT_YELLOW, SCREEN_WIDTH, SCREEN_HEIGHT_BLUE},
                       (void *)_Display.data->rearShockBuffer,
                       (void *)plotSettings,
                       PlotDrawByte);
}

void view4(void)
{
    View *newView = (View *)_Display.dataAlloc;
    view_new_inAllocatedPlace(newView, 1);
    ValSettings *topBarSettings = (ValSettings *)getSettings(0);
    *topBarSettings = (ValSettings){
        .format = "%d:%02d:%02d",
        .maxLength = 15,
        .textSize = &Font16,
        .textScale = 2,
        .offsetX = 0,
        .offsetY = 10};
    Window_new_inPlace(&newView->windows[0],
                       (Frame){0, 0, SCREEN_WIDTH, SCREEN_HEIGHT_YELLOW},
                       (void *)&(_Display.data->time),
                       (void *)topBarSettings,
                       ValDrawTime);
}

//view_new_func views_all[] = {view0, view1, view2, view3, view4};
view_new_func views_all[] = {view0, view1};
char DISPLAY_TYPES_LENGTH = sizeof(views_all) / sizeof(view_new_func);
void Display_init(SensorData *data)
{
    _Display.data = data;
    _Display.currentType = 0;
    Screen_setup();

    _Display.dataAlloc = allocData;
    views_all[_Display.currentType]();
}

void Display_setDisplayType(uint8_t type)
{
    _Display.currentType = (uint8_t)type;
}

void Display_incDisplayType()
{
    _Display.currentType = (_Display.currentType + 1) % DISPLAY_TYPES_LENGTH;
    assert(_Display.currentType >= 0 && _Display.currentType < DISPLAY_TYPES_LENGTH);

    views_all[_Display.currentType]();
}

void Display_decDisplayType()
{
    _Display.currentType = _Display.currentType > 0 ? (_Display.currentType - 1) : DISPLAY_TYPES_LENGTH - 1;
    assert(_Display.currentType >= 0 && _Display.currentType < DISPLAY_TYPES_LENGTH);

    views_all[_Display.currentType]();
}

void Display_update()
{
    Screen_clear();

    //display data on screen
    view_draw((View*)_Display.dataAlloc);

    //Screen_draw();
}

#endif