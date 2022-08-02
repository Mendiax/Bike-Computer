#include <views/display.h>
#include "views/frame.h"
#include "traces.h"
#include "display/debug.h"
#include "massert.h"

#include "views/screenfunc/common.h"
#include "views/screenfunc/plot.h"
#include "views/screenfunc/lastval.h"
#include "views/screenfunc/label.h"
#include "views/screenfunc/val.h"

#include <algorithm>
#include <inttypes.h>
#include <tuple>
#include <typeinfo>
#include <stdint.h>

#define SETTING_SIZE std::max(std::max(std::max(sizeof(LastValSettings), sizeof(PlotSettings)), sizeof(LabelSettings)), sizeof(ValSettings))
uint8_t allocData[SETTING_SIZE * MAX_NUMBER_OF_WINDOWS + sizeof(View)];

void* getSettings(unsigned id){
    assert(id < MAX_NUMBER_OF_WINDOWS);
    return allocData + sizeof(View) + id * SETTING_SIZE;
}

void add_label(const char* string, const Frame& frame, uint8_t& settingsId, uint8_t& windowId,  Align align = Align::LEFT, size_t commonLength = 0)
{
    auto valSettings = (LabelSettings*)getSettings(settingsId++);
    if(commonLength > 0)
    {
        labelSettingsNew(valSettings, frame, string, commonLength);
    }
    else
    {
        labelSettingsNew(valSettings, frame, string);
    }
    
    labelSettingsAlign(valSettings, frame, align);
    View *newView = (View *)_Display.dataAlloc;
    Window_new_inPlace(&newView->windows[windowId++],
                        valSettings,
                        LabelDraw);
}

template<typename T>
void add_value(const char* format, size_t commonLength, T* data, const Frame& frame, uint8_t& settingsId, uint8_t& windowId,  Align align = Align::LEFT)
{
    auto valSettings = (ValSettings*)getSettings(settingsId++);
    labelSettingsNew((LabelSettings *)valSettings, frame, format, commonLength);
    
    labelSettingsAlign((LabelSettings *)valSettings, frame, align);
    valSettings->data = data;
    View *newView = (View *)_Display.dataAlloc;
    Window_new_inPlace(&newView->windows[windowId++],
                        valSettings,
                        getDrawFunc(data));
}

void add_Vertical(const char* over, const char* under, const Frame& frame, uint8_t& settingsId, uint8_t& windowId,  Align align = Align::LEFT)
{
    const uint16_t str_len_max = std::max(strlen(over), strlen(under));

    Frame unitTop =     {frame.x, frame.y, frame.width, frame.height/2};
    add_label(over, unitTop, settingsId, windowId, align, str_len_max);

    Frame unitBottom =  {frame.x, frame.y + labelSettingsGetHeight((LabelSettings *)getSettings(settingsId-1)), frame.width, frame.height/2};
    add_label(under, unitBottom, settingsId, windowId, align, str_len_max);
}

template<typename T, typename Q>
void add_Vertical(const char* overFormat, size_t overCommonLength, T* overData,
                  const char* underFormat, size_t underCommonLength, Q* underData,
                  const Frame& frame, uint8_t& settingsId, uint8_t& windowId,  Align align = Align::LEFT)
{
    const uint16_t str_len_max = std::max(overCommonLength, underCommonLength);

    Frame unitTop =     {frame.x, frame.y, frame.width, frame.height/2};
    add_value(overFormat, str_len_max, overData, unitTop, settingsId, windowId, align);

    Frame unitBottom =  {frame.x, frame.y + labelSettingsGetHeight((LabelSettings *)getSettings(settingsId-1)), frame.width, frame.height/2};
    add_value(underFormat, str_len_max, underData, unitBottom, settingsId, windowId, align);
}

void add_Units(const char* over, const char* under, const Frame& frame, uint8_t& settingsId, uint8_t& windowId,  Align align = Align::LEFT)
{
    const uint16_t str_len_max = std::max(strlen(over), strlen(under));

    add_Vertical(over, under, frame, settingsId, windowId, align);

    static const char* line_template = "_____";
    static char line[6];
    memcpy(line, line_template, sizeof(line));
    line[str_len_max] = '\0'; 
    Frame unitMid =     {frame.x, frame.y,   frame.width, frame.height/2};
    add_label(line, unitMid, settingsId, windowId, align);
}

/**
 * @brief split frame into fram with len-1/len width and 1/len width
 * 
 * @param frame 
 * @param len 
 * @return auto 
 */
static auto splitFrame(const Frame& frame, uint16_t length, bool align_right=false)
{
    massert(length > 1, "length should be greater than 1\n");
    uint16_t widthPerChar = frame.width / length;
    const sFONT* char_font;
    uint8_t char_scale;
    getFontSize(widthPerChar, frame.height, &char_font, &char_scale);
    uint16_t char_width = char_font->width * char_scale;
    massert(char_width <= widthPerChar, "char becomes bigger %d > %d\n", char_width, widthPerChar);
    massert(length * char_width <= DISPLAY_WIDTH, "length outside of display width range %d > %d\n", length * char_width, DISPLAY_WIDTH);

    uint16_t height = char_font->height * char_scale;
    //auto offset = (frame.width / length);
    Frame f1 = {frame.x, frame.y, char_width * (length - 1), height};
    Frame f2 = {f1.x + f1.width, frame.y, char_width, height};
    
    TRACE_DEBUG(1, TRACE_VIEWS, "Frame splited %s and %s\n", frame_to_string(f1).c_str(), frame_to_string(f2).c_str());

    //assert(frame.width >= (f1.width + f2.width);
    massert(frame.width >= (f1.width + f2.width), "offset will be negative, f1 and f2 width are not correct\n");
    uint16_t offset = frame.width - (f1.width + f2.width);
    if(align_right)
    {
        printf("offset = %" PRIu16 " \n", offset);
        f1.x += offset;
        f2.x += offset;
    }
    else
    {
        f2.width += offset;
    }
    TRACE_DEBUG(1, TRACE_VIEWS, "Frame splited %s and %s\n", frame_to_string(f1).c_str(), frame_to_string(f2).c_str());
    return std::make_tuple(f1,f2);
}

template<typename T>
void addValueUnitsVertical(const char* format, size_t commonLength, T* data,
                           const char* over, const char* under,
                           const Frame& frame, 
                           uint8_t& settingsId, uint8_t& windowId,
                           Align align = Align::LEFT, bool alignRight = true)
{
    auto [frameValue, frameUnits] = splitFrame(frame, commonLength + 1, alignRight);
    add_value(format, commonLength, data, frameValue, settingsId, windowId);
    add_Units(over, under, frameUnits, settingsId, windowId, align);
}
template<typename T, typename Q, typename P>
void addValueValuesVertical(const char* format, size_t commonLength, T* data,
                            const char* overFormat, size_t overCommonLength, Q* overData,
                            const char* underFormat, size_t underCommonLength, P* underData,
                            const Frame& frame, 
                            uint8_t& settingsId, uint8_t& windowId,
                            Align align = Align::LEFT, bool alignRight = true)
{
    auto [frameValue, frameUnits] = splitFrame(frame, commonLength + 1, alignRight);
    add_value(format, commonLength, data, frameValue, settingsId, windowId);
    add_Vertical(overFormat, overCommonLength, overData,
                 underFormat, underCommonLength, underData,
                 frameUnits, settingsId, windowId, align);
}

/*

    speed, distance 
*/
void view0(void)
{
    
    uint8_t numberOfWindows = 6;
    View *newView = (View *)_Display.dataAlloc;
    view_new_inAllocatedPlace(newView, 0); // TODO fix this xd no of windows is not needed here
    uint8_t settingsId = 0;
    uint8_t windowId = 0;

    Frame unit = {0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT/2}; 

    addValueUnitsVertical("%2.0f", 2, &_Display.data->speed.velocity, "km","h", unit, 
                          settingsId, windowId, Align::CENTER, true);
    
    Frame distanceFrame = {0, DISPLAY_HEIGHT/2, DISPLAY_WIDTH, DISPLAY_HEIGHT/2};
    addValueValuesVertical("%3" PRIu16, 3, &_Display.data->speed.distance,
                           "%2" PRIu8, 2, &_Display.data->speed.distanceDec,
                           "km", 2, (void*)0,
                           distanceFrame,
                           settingsId, windowId, Align::CENTER, true);
    
    newView->numberOfWindows = windowId;
    //DEBUG_OLED_ASSERT(numberOfWindows == windowId, "Too many windows %u\n", windowId);
    //DEBUG_OLED_ASSERT(numberOfWindows == settingsId, "Too many settings %u\n", settingsId);

    // TODO static assert
    DEBUG_OLED_ASSERT(MAX_NUMBER_OF_WINDOWS >= numberOfWindows, 
                      "Increase macro MAX_NUMBER_OF_WINDOWS-> %u", 
                      numberOfWindows);

    TRACE_DEBUG(4, TRACE_VIEWS, "view0 setup finished \n%s", "");
}


/* 
    max, avg display
*/
void view1(void)
{
    uint8_t numberOfWindows = 4;
    uint8_t settingsId = 0;
    uint8_t windowId = 0;

    // View *newView = (View *)_Display.dataAlloc;
    // view_new_inAllocatedPlace(newView, numberOfWindows);

    // ValSettings *maxSpeedSettings = (ValSettings *)getSettings(settingsId++);
    // *maxSpeedSettings = (ValSettings){
    //     .format = "%2.0f",
    //     .maxLength = 6,
    //     .font = &Font24,
    //     .textScale = 2,
    //     .offsetX = 0,
    //     .offsetY = 44};
    // Window_new_inPlace(&newView->windows[windowId++],
    //                    (Frame){0, 0, SCREEN_WIDTH/2, SCREEN_HEIGHT},
    //                    (void *)&(_Display.data->speed.velocityMax),
    //                    (void *)maxSpeedSettings,
    //                    drawFormatFloat);
    // Window_new_inPlace(&newView->windows[windowId++],
    //                    (Frame){SCREEN_WIDTH/2, 0, SCREEN_WIDTH/2, SCREEN_HEIGHT},
    //                    (void *)&(_Display.data->speed.avgGlobal),
    //                    (void *)maxSpeedSettings,
    //                    drawFormatFloat);

    // /*top header*/
    // LabelSettings *header2Settings = (LabelSettings *)getSettings(settingsId++);
    // *header2Settings = (LabelSettings){
    //     .string = "max   avg",
    //     .font = &Font20,
    //     .offsetX = 2,
    //     .offsetY = 20};
    // Window_new_inPlace(&newView->windows[windowId++],
    //                    (Frame){0, 0, SCREEN_WIDTH, SCREEN_HEIGHT},
    //                    (void *)0,
    //                    (void *)header2Settings,
    //                    LabelDraw);

    // ValSettings *topBarSettings = (ValSettings *)getSettings(settingsId++);
    // *topBarSettings = (ValSettings){
    //     .text.string = "%d:%02d:%02d",
    //     .text.str_len = 15,
    //     .text.font = &Font16,
    //     .text.scale = 2,
    //     .text.offsetX = 0,
    //     .text.offsetY = 100};
    // Window_new_inPlace(&newView->windows[windowId++],
    //                    (Frame){0, 0, SCREEN_WIDTH, SCREEN_HEIGHT_YELLOW},
    //                    (void *)&(_Display.data->time),
    //                    (void *)topBarSettings,
    //                    ValDrawTime);


    /*BAT LEVEL*/
   /* ValSettings *lipoSettings = (ValSettings *)getSettings(settingsId++);
    *lipoSettings = (ValSettings){
        .format = "%d",
        .maxLength = 6,
        .font = &Font20,
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


// /*
//     speed plot
// */
// void view2(void)
// {
//     // View *newView = (View *)_Display.dataAlloc;
//     // view_new_inAllocatedPlace(newView, 1);
//     // PlotSettings *plotSettings = (PlotSettings *)getSettings(0);
//     // *plotSettings = {
//     //     .min = 0,
//     //     .max = 30,
//     //     .autoMax = true,
//     //     .autoMin = false,
//     //     .offset = 5};
//     // Window_new_inPlace(&newView->windows[0],
//     //                    (Frame){0, 0, SCREEN_WIDTH, SCREEN_HEIGHT},
//     //                    (void *)_Display.data->speed.speedBuffer,
//     //                    (void *)plotSettings,
//     //                    PlotDrawByte);
// }


// /*
//     rear shock plot
// */
// void view3(void)
// {
//     View *newView = (View *)_Display.dataAlloc;
//     view_new_inAllocatedPlace(newView, 2);
//     LastValSettings *topBarSettings = (LastValSettings *)getSettings(0);
//     *topBarSettings = (LastValSettings){
//         .format = "rear shock: %2d",
//         .maxLength = 15,
//         .textSize = &Font12,
//         .textScale = 1,
//         .offsetX = 0,
//         .offsetY = 0};
//     Window_new_inPlace(&newView->windows[0],
//                        (Frame){0, 0, SCREEN_WIDTH, SCREEN_HEIGHT_YELLOW},
//                        (void *)_Display.data->rearShockBuffer,
//                        (void *)topBarSettings,
//                        LastValDrawByte);

    
//     PlotSettings *plotSettings = (PlotSettings *)getSettings(1);
//     *plotSettings = {
//         .min = 0,
//         .max = 100,
//         .autoMax = true,
//         .autoMin = true,
//         .offset = 5};
//     Window_new_inPlace(&newView->windows[1],
//                        (Frame){0, SCREEN_HEIGHT_YELLOW, SCREEN_WIDTH, SCREEN_HEIGHT_BLUE},
//                        (void *)_Display.data->rearShockBuffer,
//                        (void *)plotSettings,
//                        PlotDrawByte);
// }

// void view4(void)
// {
//     View *newView = (View *)_Display.dataAlloc;
//     view_new_inAllocatedPlace(newView, 1);
//     ValSettings *topBarSettings = (ValSettings *)getSettings(0);
//     *topBarSettings = (ValSettings){
//         .format = "%d:%02d:%02d",
//         .maxLength = 15,
//         .textSize = &Font16,
//         .textScale = 2,
//         .offsetX = 0,
//         .offsetY = 10};
//     Window_new_inPlace(&newView->windows[0],
//                        (Frame){0, 0, SCREEN_WIDTH, SCREEN_HEIGHT_YELLOW},
//                        (void *)&(_Display.data->time),
//                        (void *)topBarSettings,
//                        ValDrawTime);
// }