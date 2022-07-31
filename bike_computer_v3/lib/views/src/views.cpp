#include <views/display.h>
#include "views/frame.h"
#include "traces.h"

#include "views/screenfunc/common.h"
#include <views/screenfunc/plot.h>
#include <views/screenfunc/lastval.h>
#include <views/screenfunc/label.h>
#include <views/screenfunc/val.h>

#include <algorithm>
#include "display/debug.h"
#include <inttypes.h>
#include <tuple>
#include <typeinfo>

// TODO create simple way of creating window of given size and whit should be printing

#define SETTING_SIZE std::max(std::max(std::max(sizeof(LastValSettings), sizeof(PlotSettings)), sizeof(LabelSettings)), sizeof(ValSettings))
uint8_t allocData[SETTING_SIZE * MAX_NUMBER_OF_WINDOWS + sizeof(View)];

void* getSettings(unsigned id){
    assert(id < MAX_NUMBER_OF_WINDOWS);
    return allocData + sizeof(View) + id * SETTING_SIZE;
}

static void fitToSpace(TextSettings* settings, const Frame& frame)
{
    // we need to calculate
    // font size and scale
    //  based on frame size and text length
    size_t length = settings->str_len;
    u_int16_t widthPerChar = frame.width / length;
    uint8_t scale = 0;

    getFontSizePreferBiggerFonts(widthPerChar, frame.height, &settings->font, &settings->scale);
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
    u_int16_t widthPerChar = frame.width / length;
    const sFONT* char_font;
    uint8_t char_scale;
    getFontSize(widthPerChar, frame.height, &char_font, &char_scale);
    uint16_t char_width = char_font->width * char_scale;

    //auto offset = (frame.width / length);
    Frame f1 = {frame.x, frame.y, char_width * (length - 1), frame.height};
    Frame f2 = {f1.x + f1.width, frame.y, char_width, frame.height};

    uint16_t offset = frame.width - (f1.width + f2.width);
    if(align_right)
    {
        f1.x += offset;
        f2.x += offset;
    }
    else
    {
        f2.width += offset;
    }
    return std::make_tuple(f1,f2);
}

static void createVertical(const Frame& frame,
                           const DisplayData& value_over,
                           const DisplayData& value_under,
                           uint8_t& settingsId, uint8_t& windowId)
{
    View *newView = (View *)_Display.dataAlloc;
    //Frame kph = {17*3*2 - 2, 10 + 20, 0, 0};
    //const sFONT* kphFont = &Font20;

    const uint16_t str_len_max = std::max(strlen(over), strlen(under));
    TextSettings defaultSettings = {0};
    defaultSettings.string = strlen(over) >= strlen(under) ? over : under;
    defaultSettings.str_len = strlen(defaultSettings.string);
    Frame halfFrame = frame;
    halfFrame.height /= 4;
    fitToSpace(&defaultSettings, halfFrame);
    TRACE_DEBUG(3, TRACE_VIEWS, "halfFrame.height %" PRIu16 " .width %" PRIu16 " \n", halfFrame.height, halfFrame.width);
    TRACE_DEBUG(3, TRACE_VIEWS, "font height %" PRIu16 " .width %" PRIu16 " \n", 
                                defaultSettings.font->height * defaultSettings.scale,
                                defaultSettings.font->width * defaultSettings.scale);
    const uint16_t char_width = (defaultSettings.font->width * defaultSettings.scale);
    const uint16_t label_width = str_len_max * (char_width);




    const uint8_t line_offset = 0;
    {
        const uint8_t offset_x_axis = (label_width - strlen(over) * (char_width)) / 2;
        LabelSettings *headerSettings0 = (LabelSettings *)getSettings(settingsId++);
        *headerSettings0 = (LabelSettings){
            .text = (TextSettings){
                .string = over,
                .font = defaultSettings.font,
                .str_len = strlen(over),
                .offsetX = offset_x_axis,
                .offsetY = 0,
                .scale = defaultSettings.scale
                }
            };
        Window_new_inPlace(&newView->windows[windowId++],
                        frame,
                        (void *)0,
                        (void *)headerSettings0,
                        LabelDraw);
    }
    {
        const uint8_t offset_x_axis = (label_width - strlen(under) * (char_width)) / 2;
        LabelSettings *headerSettings2 = (LabelSettings *)getSettings(settingsId++);
        *headerSettings2 = (LabelSettings){
            .text = (TextSettings){
                .string = under,
                .font = defaultSettings.font,
                .str_len = strlen(under),
                .offsetX = offset_x_axis,
                .offsetY = 0,
                .scale = defaultSettings.font->height * defaultSettings.scale + line_offset
            }
        };
        Window_new_inPlace(&newView->windows[windowId++],
                        frame,
                        (void *)0,
                        (void *)headerSettings2,
                        LabelDraw);
    }
}
{
    {
    View *newView = (View *)_Display.dataAlloc;
    //Frame kph = {17*3*2 - 2, 10 + 20, 0, 0};
    //const sFONT* kphFont = &Font20;

    const uint16_t str_len_max = std::max(value_over.format_length, value_under.format_length);
    TextSettings defaultSettings = {0};
    defaultSettings.str_len = str_len_max;
    Frame halfFrame = frame;
    halfFrame.height /= 4;
    fitToSpace(&defaultSettings, halfFrame);
    TRACE_DEBUG(3, TRACE_VIEWS, "halfFrame.height %" PRIu16 " .width %" PRIu16 " \n", halfFrame.height, halfFrame.width);
    TRACE_DEBUG(3, TRACE_VIEWS, "font height %" PRIu16 " .width %" PRIu16 " \n", 
                                defaultSettings.font->height * defaultSettings.scale,
                                defaultSettings.font->width * defaultSettings.scale);
    const uint16_t char_width = (defaultSettings.font->width * defaultSettings.scale);
    const uint16_t label_width = str_len_max * (char_width);


    TODO change it to make it first split inot windows and then fit inside them and it should be more generic

    windos = split window ()
    
    write into window(seeting, window)
    {
        fit to space (settings, widnow)
        add window

    }


    const uint8_t line_offset = 0;
    {
        const uint8_t offset_x_axis = (label_width - strlen(over) * (char_width)) / 2;
        LabelSettings *headerSettings0 = (LabelSettings *)getSettings(settingsId++);
        *headerSettings0 = (LabelSettings){
            .text = (TextSettings){
                .string = over,
                .font = defaultSettings.font,
                .str_len = strlen(over),
                .offsetX = offset_x_axis,
                .offsetY = 0,
                .scale = defaultSettings.scale
                }
            };
        Window_new_inPlace(&newView->windows[windowId++],
                        frame,
                        (void *)0,
                        (void *)headerSettings0,
                        LabelDraw);
    }
    {
        const uint8_t offset_x_axis = (label_width - strlen(under) * (char_width)) / 2;
        LabelSettings *headerSettings2 = (LabelSettings *)getSettings(settingsId++);
        *headerSettings2 = (LabelSettings){
                .text = (TextSettings){
                .string = under,
                .font = defaultSettings.font,
                .str_len = strlen(under),
                .offsetX = offset_x_axis,
                .offsetY = defaultSettings.font->height * defaultSettings.scale + line_offset,
                .scale = defaultSettings.scale
                }
        };
        Window_new_inPlace(&newView->windows[windowId++],
                        frame,
                        (void *)0,
                        (void *)headerSettings2,
                        LabelDraw);
    }
}

static void createUnit(const Frame& frame,
                       const char* over, const char* under,
                       uint8_t& settingsId, uint8_t& windowId)
{
    View *newView = (View *)_Display.dataAlloc;
    //Frame kph = {17*3*2 - 2, 10 + 20, 0, 0};
    //const sFONT* kphFont = &Font20;

    const uint16_t str_len_max = std::max(strlen(over), strlen(under));
    TextSettings defaultSettings = {0};
    //defaultSettings.string = strlen(over) >= strlen(under) ? over : under;
    defaultSettings.str_len = str_len_max;
    Frame halfFrame = frame;
    halfFrame.height /= 4;
    fitToSpace(&defaultSettings, halfFrame);
    TRACE_DEBUG(3, TRACE_VIEWS, "halfFrame.height %" PRIu16 " .width %" PRIu16 " \n", halfFrame.height, halfFrame.width);
    TRACE_DEBUG(3, TRACE_VIEWS, "font height %" PRIu16 " .width %" PRIu16 " \n", 
                                defaultSettings.font->height * defaultSettings.scale,
                                defaultSettings.font->width * defaultSettings.scale);
    const uint16_t char_width = (defaultSettings.font->width * defaultSettings.scale);
    const uint16_t label_width = str_len_max * (char_width);




    const uint8_t line_offset = 0;
    {
        const uint8_t offset_x_axis = (label_width - strlen(over) * (char_width)) / 2;
        LabelSettings *headerSettings0 = (LabelSettings *)getSettings(settingsId++);
        *headerSettings0 = (LabelSettings){
            .text = (TextSettings){
                .string = over,
                .font = defaultSettings.font,
                .str_len = strlen(over),
                .offsetX = offset_x_axis,
                .offsetY = 0,
                .scale = defaultSettings.scale
                }
            };
        Window_new_inPlace(&newView->windows[windowId++],
                        frame,
                        (void *)0,
                        (void *)headerSettings0,
                        LabelDraw);
    }
    {
        const uint8_t offset_x_axis = (label_width - strlen(under) * (char_width)) / 2;
        LabelSettings *headerSettings2 = (LabelSettings *)getSettings(settingsId++);
        *headerSettings2 = (LabelSettings){
                .text = (TextSettings){
                .string = under,
                .font = defaultSettings.font,
                .str_len = strlen(under),
                .offsetX = offset_x_axis,
                .offsetY = defaultSettings.font->height * defaultSettings.scale + line_offset,
                .scale = defaultSettings.scale
                }
        };
        Window_new_inPlace(&newView->windows[windowId++],
                        frame,
                        (void *)0,
                        (void *)headerSettings2,
                        LabelDraw);
    }
    {
        static const char* line_template = "_____";
        static char line[6];
        memcpy(line, line_template, sizeof(line));
        line[str_len_max] = '\0'; 
        //const uint8_t offset_x_axis = (frame.width - strlen(line) * (char_width)) / 2;
        LabelSettings *headerSettings1 = (LabelSettings *)getSettings(settingsId++);
        *headerSettings1 = (LabelSettings){
            .text = (TextSettings){
                .string = line,
                .font = defaultSettings.font,
                .str_len = strlen(line),
                .offsetX = 0,
                .offsetY = line_offset,
                .scale = defaultSettings.scale
            }
        };
        Window_new_inPlace(&newView->windows[windowId++],
                            frame,
                            (void *)0,
                            (void *)headerSettings1,
                            LabelDraw);
    }
}

void displayValueWithVerticalFraction(const Frame& frame,
                                      const DisplayData& value,
                                      const DisplayData& value_over,
                                      const DisplayData& value_under,
                                      bool align_right,
                                      View *newView,
                                      uint8_t& settingsId,
                                      uint8_t& windowId
                                      )
{
    ValSettings *valSettings = (ValSettings *)getSettings(settingsId++);
    *valSettings = (ValSettings){
        .text = (TextSettings){
            .string = value.format,
            .font = 0,
            .str_len = value.format_length
        }
    }; 
    auto [frame_val, frame_unit] = splitFrame(frame, valSettings->text.str_len + 1, align_right); // +1 for units
    TRACE_DEBUG(1, TRACE_VIEWS, "Frame splited %s and %s\n", frame_to_string(frame_val).c_str(), frame_to_string(frame_unit).c_str());

    fitToSpace(&valSettings->text, frame_val);
    TRACE_DEBUG(2, TRACE_VIEWS, "Frame val scaled %s\n", "");
    Window_new_inPlace(&newView->windows[windowId++],
                        frame_val,
                        value.data,
                        valSettings,
                        value.func);

    createVertical(frame_unit, value_over, value_under, settingsId, windowId);
    TRACE_DEBUG(3, TRACE_VIEWS, "unit created %s\n", "");
}

void displayValueUnitsFraction(const Frame& frame,
                       const char* format,
                       uint16_t format_length,
                       bool align_right,
                       void* data,
                       drawFunc_p func,
                       const char* unit_over,
                       const char* unit_under,
                       View *newView,
                       uint8_t& settingsId,
                       uint8_t& windowId
                       )
{
    ValSettings *valSettings = (ValSettings *)getSettings(settingsId++);
    *valSettings = (ValSettings){
        .text = (TextSettings){
            .string = format,
            .font = 0,
            .str_len = format_length
        }
    }; 
    auto [frame_val, frame_unit] = splitFrame(frame, valSettings->text.str_len + 1, align_right); // +1 for units
    TRACE_DEBUG(1, TRACE_VIEWS, "Frame splited %s and %s\n", frame_to_string(frame_val).c_str(), frame_to_string(frame_unit).c_str());

    fitToSpace(&valSettings->text, frame_val);
    TRACE_DEBUG(2, TRACE_VIEWS, "Frame val scaled %s\n", "");
    Window_new_inPlace(&newView->windows[windowId++],
                frame_val,
                (void *)data,
                (void *)valSettings,
                func);

    createUnit(frame_unit, unit_over,unit_under,settingsId, windowId);
    TRACE_DEBUG(3, TRACE_VIEWS, "unit created %s\n", "");
}
/*

    speed, distance 
*/
void view0(void)
{
    uint8_t numberOfWindows = 4;
    View *newView = (View *)_Display.dataAlloc;
    view_new_inAllocatedPlace(newView, 0); // TODO fix this xd no of windows is not needed here
    uint8_t settingsId = 0;
    uint8_t windowId = 0;

    displayValueUnitsFraction((Frame){0,0,DISPLAY_WIDTH, DISPLAY_HEIGHT / 2},
                              "%2.0f", 2,
                              true,
                              &_Display.data->speed.velocity,
                              drawFormatFloat,
                              "km", "h",
                              newView,settingsId, windowId);
    
    
    // // --- distance ---
    // uint16_t offset =  DISPLAY_HEIGHT / 2;//valSettings->font->height * valSettings->textScale + 20;
    // ValSettings *valSettings2 = (ValSettings *)getSettings(settingsId++);
    // *valSettings2 = (ValSettings){
    //     .text.string = "%3" PRIu16,
    //     .text.str_len = 3,
    //     .text.font = &Font24,
    //     .text.scale = 2,
    //     .text.offsetX = 2,
    //     .text.offsetY = 0};
    // Window_new_inPlace(&newView->windows[windowId++],
    //                    (Frame){0, offset, SCREEN_WIDTH/2, SCREEN_HEIGHT/2},
    //                    (void *)&(_Display.data->speed.distance),
    //                    (void *)valSettings2,
    //                    drawFormatInt16);
    
    // Frame km = {17*3*2 - 2, offset, 0, 0};
    // const sFONT* kmFont = &Font20;

    // LastValSettings *valSettings1 = (LastValSettings *)getSettings(settingsId++);
    // *valSettings1 = (LastValSettings){
    //     .text.string = "%02" PRIu8,
    //     .text.str_len = 2,
    //     .text.font = kmFont,
    //     .text.scale = 1,
    //     .text.offsetX = 0,
    //     .text.offsetY = 0};
    // Window_new_inPlace(&newView->windows[windowId++],
    //                    km,
    //                    (void *)&(_Display.data->speed.distanceDec),
    //                    (void *)valSettings1,
    //                    drawFormatInt8);

    // LabelSettings *headerSettings3 = (LabelSettings *)getSettings(settingsId++);
    // *headerSettings3 = (LabelSettings){
    //     .text.string = "km",
    //     .text.str_len = strlen(headerSettings3.text.string),
    //     .text.font = kmFont,
    //     .text.scale = 1,
    //     .text.offsetX = 0,
    //     .text.offsetY = kmFont->height
    //     };
    // Window_new_inPlace(&newView->windows[windowId++],
    //                    km,
    //                    (void *)0,
    //                    (void *)headerSettings3,
    //                    LabelDraw);
    
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