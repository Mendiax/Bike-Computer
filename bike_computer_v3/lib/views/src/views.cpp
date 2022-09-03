#include <views/display.h>
#include "views/frame.h"
#include "traces.h"
#include "display/debug.h"
#include "massert.h"
#include "common_types.h"

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
#include <iostream>

//#define SETTING_SIZE std::max(std::max(std::max(sizeof(LastValSettings), sizeof(PlotSettings)), sizeof(LabelSettings)), sizeof(ValSettings))
//uint8_t allocData[SETTING_SIZE * MAX_NUMBER_OF_WINDOWS + sizeof(View)];
//
//void* getSettings(unsigned id){
//    assert(id < MAX_NUMBER_OF_WINDOWS);
//    return allocData + sizeof(View) + id * SETTING_SIZE;
//}

void add_label(const char* string, const Frame& frame, Align align = Align::LEFT, size_t commonLength = 0)
{
    LabelSettings valSettings;
    if(commonLength > 0)
    {
        labelSettingsNew(&valSettings, frame, string, commonLength);
    }
    else
    {
        labelSettingsNew(&valSettings, frame, string);
    }
    
    labelSettingsAlign(&valSettings, frame, align);
    view_addNewWindow(&_Display.view, {valSettings, LabelDraw});
    
}

template<typename T>
void add_value(const char* format, size_t commonLength, T* data, const Frame& frame, Align align = Align::LEFT)
{
    Window newWindow;
    newWindow.updateFunc_p = getDrawFunc(data);
    labelSettingsNew(&newWindow.settings.label, frame, format, commonLength);
    
    labelSettingsAlign(&newWindow.settings.label, frame, align);
    newWindow.settings.val.data = data;
    view_addNewWindow(&_Display.view, newWindow);
}

void add_Vertical(const char* over, const char* under, const Frame& frame, Align align = Align::LEFT)
{
    const uint_fast16_t str_len_max = std::max(strlen(over), strlen(under));
    const uint_fast16_t half_height = frame.height >> 1;
    const Frame unitTop = {frame.x, frame.y, frame.width, (uint16_t)half_height};
    add_label(over, unitTop,  align, str_len_max);

    const uint16_t top_unit_max_y = frame.y + labelSettingsGetHeight(&view_getPreviousWindow(&_Display.view)->settings.label);
    const Frame unitBottom = {frame.x, top_unit_max_y,
                              frame.width, (uint16_t)half_height};
    add_label(under, unitBottom,  align, str_len_max);
}

template<typename T, typename Q>
void add_Vertical(const char* overFormat, size_t overCommonLength, T* overData,
                  const char* underFormat, size_t underCommonLength, Q* underData,
                  const Frame& frame,   Align align = Align::LEFT)
{
    const uint_fast16_t str_len_max = std::max(overCommonLength, underCommonLength);
    const uint_fast16_t half_height = frame.height >> 1;

    const Frame unitTop =     {frame.x, frame.y, frame.width, (uint16_t)half_height};
    add_value(overFormat, str_len_max, overData, unitTop,  align);

    const uint_fast16_t top_unit_height = labelSettingsGetHeight(&view_getPreviousWindow(&_Display.view)->settings.label);
    const Frame unitBottom =  {frame.x, frame.y + (uint16_t)top_unit_height,
                         frame.width, (uint16_t)half_height};
    add_value(underFormat, str_len_max, underData, unitBottom,  align);
}

void add_Units(const char* over, const char* under, const Frame& frame, Align align = Align::LEFT)
{
    const uint16_t str_len_max = std::max(strlen(over), strlen(under));
    const uint_fast16_t half_height = frame.height >> 1;

    add_Vertical(over, under, frame,  align);

    static const char* line_template = "_____";
    static char line[6];
    memcpy(line, line_template, sizeof(line));
    line[str_len_max] = '\0'; 
    const Frame unitMid =     {frame.x, frame.y,   frame.width, (uint16_t)half_height};
    add_label(line, unitMid,  align);
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
    const uint16_t widthPerChar = frame.width / length;
    const sFONT* char_font;
    uint8_t char_scale;
    getFontSize(widthPerChar, frame.height, &char_font, &char_scale);
    const uint16_t char_width = char_font->width * char_scale;
    massert(char_width <= widthPerChar, "char becomes bigger %d > %d\n", char_width, widthPerChar);
    massert(length * char_width <= DISPLAY_WIDTH, "length outside of display width range %d > %d\n", length * char_width, DISPLAY_WIDTH);

    uint16_t height = char_font->height * char_scale;
    //auto offset = (frame.width / length);
    const uint_fast16_t f1_width = char_width * (length - 1u); 
    Frame f1 = {frame.x, frame.y, (uint16_t)f1_width, height};
    const uint_fast16_t f2_x_offset = f1.x + f1.width;  
    Frame f2 = {(uint16_t) f2_x_offset, frame.y, char_width, height};
    
    TRACE_DEBUG(1, TRACE_VIEWS, "Frame splited %s and %s\n", frame_to_string(f1).c_str(), frame_to_string(f2).c_str());

    //assert(frame.width >= (f1.width + f2.width);
    massert(frame.width >= (f1.width + f2.width), "offset will be negative, f1 and f2 width are not correct\n");
    const uint16_t offset = frame.width - (f1.width + f2.width);
    if(align_right)
    {
        //PRINTF("offset = %" PRIu16 " \n", offset);
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
/**
 * @brief splits frame into 2 frames with half of width
 * 
 * @param frame 
 * @return constexpr auto 
 */
static constexpr auto split_vertical(const Frame& frame)
{
    const uint16_t half_width = frame.width / 2;
    const Frame f1 = {frame.x, frame.y, half_width, frame.height};
    const Frame f2 = {frame.x + half_width, frame.y, half_width, frame.height};
    return std::make_tuple(f1,f2);
}
/**
 * @brief splits frame into 2 frames with half of height
 * 
 * @param frame 
 * @return constexpr auto 
 */
static constexpr auto split_horizontal(const Frame& frame)
{
    const uint16_t half_height = frame.height / 2;
    const Frame f1 = {frame.x, frame.y, frame.width, half_height};
    const Frame f2 = {frame.x, frame.y + half_height, frame.width, half_height};
    
    return std::make_tuple(f1,f2);
}



template<typename T>
void addValueUnitsVertical(const char* format, size_t commonLength, T* data,
                           const char* over, const char* under,
                           const Frame& frame, 
                           Align align = Align::LEFT, bool alignRight = true)
{
    auto [frameValue, frameUnits] = splitFrame(frame, commonLength + 1, alignRight);
    add_value(format, commonLength, data, frameValue);
    add_Units(over, under, frameUnits,  align);
}
template<typename T, typename Q, typename P>
void addValueValuesVertical(const char* format, size_t commonLength, T* data,
                            const char* overFormat, size_t overCommonLength, Q* overData,
                            const char* underFormat, size_t underCommonLength, P* underData,
                            const Frame& frame,                      
                            Align align = Align::LEFT, bool alignRight = true)
{
    auto [frameValue, frameUnits] = splitFrame(frame, commonLength + 1, alignRight);
    add_value(format, commonLength, data, frameValue);
    add_Vertical(overFormat, overCommonLength, overData,
                 underFormat, underCommonLength, underData,
                 frameUnits,  align);
}

#define TOP_BAR_HEIGHT (DISPLAY_HEIGHT / 10)

void top_bar()
{
    Frame bar_frame = {0, 0, DISPLAY_WIDTH, TOP_BAR_HEIGHT};
    add_value("/\\%3d" PRIu8, BAT_LEVEL_LABEL_LENGTH, &_Display.data->lipo, bar_frame, Align::RIGHT);
}

static constexpr Frame get_frame_bar()
{
    return {0, TOP_BAR_HEIGHT, DISPLAY_WIDTH, DISPLAY_HEIGHT - TOP_BAR_HEIGHT};
}

static inline constexpr uint16_t get_frame_top_y(const Frame frame)
{
    massert(DISPLAY_HEIGHT >= frame.y + frame.height, "no space left Y:%" PRIu16  " H:%" PRIu16 "\n", frame.y, frame.height);
    return frame.y + frame.height;
}

static inline constexpr uint16_t get_height_left(const Frame frame)
{
    massert(DISPLAY_HEIGHT >= get_frame_top_y(frame), "no space left\n");
    return DISPLAY_HEIGHT - get_frame_top_y(frame);
}

/*

    speed, distance 
*/
void view0(void)
{
    view_new_inAllocatedPlace(&_Display.view);
    top_bar();

    // const auto frame = get_frame_bar();

    // const auto [frame_speed, frame_distance] = split_horizontal(); 

    Frame speed = {0, TOP_BAR_HEIGHT, DISPLAY_WIDTH, DISPLAY_HEIGHT/2};

    addValueUnitsVertical("%2.0f", 2, &_Display.data->speed.velocity, "km","h", speed, 
                           Align::CENTER, true);
    
    Frame distanceFrame = {0, (speed.y + speed.height), DISPLAY_WIDTH, DISPLAY_HEIGHT/2};
    addValueValuesVertical("%3" PRIu16, 3, &_Display.data->speed.distance,
                           "%02" PRIu8, 2, &_Display.data->speed.distanceDec,
                           "km", 2, (void*)0,
                           distanceFrame,
                            Align::CENTER, true);

    TRACE_DEBUG(4, TRACE_VIEWS, "view0 setup finished \n%s", "");
}


/* 
    max, avg display
*/
void view1(void)
{
    view_new_inAllocatedPlace(&_Display.view);
    top_bar();

    Frame max_avg_bar = {0, TOP_BAR_HEIGHT, DISPLAY_WIDTH, DISPLAY_HEIGHT/5};
    Frame max_avg_val = {0, get_frame_top_y(max_avg_bar), DISPLAY_WIDTH, 100};
    Frame time_passed = {0, get_frame_top_y(max_avg_val), DISPLAY_WIDTH, get_height_left(max_avg_val)};

    auto [bar_max,bar_avg] = split_vertical(max_avg_bar);
    add_label("max", bar_max, Align::CENTER);
    add_label("avg", bar_avg, Align::CENTER);


    auto [max,avg] = split_vertical(max_avg_val);
    add_value("%2.0f",2,&_Display.data->speed.velocityMax, max, Align::CENTER);

    auto [avg_curr, avg_global] = split_horizontal(avg);
    add_value("%4.1f",4,&_Display.data->speed.avg, avg_curr, Align::LEFT);
    add_value("%4.1f",4,&_Display.data->speed.avg_global, avg_global, Align::LEFT);


    // Frame time_passed = {0, bottomLeft.y + bottomLeft.height, DISPLAY_WIDTH, DISPLAY_HEIGHT - time_passed.y};
    add_value("%2d:%02d:%02d", 8 ,(mtime_t*)&_Display.data->speed.drive_time, time_passed, Align::CENTER);


    TRACE_DEBUG(4, TRACE_VIEWS, "view1 setup finished \n%s", "");
}

void view2(void)
{
    view_new_inAllocatedPlace(&_Display.view);
    top_bar();
    auto frame  = get_frame_bar();
    auto [gps, gsm] = split_horizontal(frame);


    auto [gps_speed_height, gps_pos] = split_horizontal(gps);
    auto [gps_speed, gps_height] = split_vertical(gps_speed_height);
    auto [gps_lat, gps_long] = split_vertical(gps_pos);
    auto [signal, bmp] = split_vertical(gsm);
    auto [sat, sat2] = split_horizontal(signal);
    auto [temp, press] = split_horizontal(bmp);



    //Frame topLeft = {0,TOP_BAR_HEIGHT, DISPLAY_WIDTH/2, DISPLAY_HEIGHT/2};
    //Frame topRight = {DISPLAY_WIDTH/2,topLeft.y, DISPLAY_WIDTH/2, topLeft.height};
    addValueUnitsVertical("%2.0f", 2, &_Display.data->gps_data.speed, "km","h", gps_speed, 
                           Align::CENTER, true);
    //add_value("%3.0f",3,&_Display.data->gps_data.msl, gps_height, Align::CENTER);
    add_value("%3.0fm",3,&_Display.data->altitude, gps_height, Align::CENTER);

    //Frame bottomLeft ={0, get_frame_top_y(topLeft), topLeft.width, DISPLAY_HEIGHT - bottomLeft.y};
    //Frame bottomRight ={topRight.x,get_frame_top_y(topRight), topRight.width, bottomRight.height};
    add_value("%6.3f",6,&_Display.data->gps_data.lat, gps_lat, Align::CENTER);
    add_value("%6.3f",6,&_Display.data->gps_data.lon, gps_long, Align::CENTER);


    add_value("%2" PRIu8,2,&_Display.data->gps_data.sat, sat, Align::CENTER);
    add_value("%2" PRIu8,2,&_Display.data->gps_data.sat2, sat2, Align::CENTER);

    add_value("% 3.0fC",3,&_Display.data->weather.temperature, temp, Align::CENTER);
    //add_value("%4f",4,&_Display.data->gps_data.sat2, sat2, Align::CENTER);
}

void view3(void)
{
    view_new_inAllocatedPlace(&_Display.view);
    top_bar();

    auto frame  = get_frame_bar();
    auto [frame_hour, frame_date] = split_horizontal(frame);
    add_value("",TIMES_LABEL_LENGTH,&_Display.data->hour, frame_hour, Align::CENTER);
    add_value("",TIMES_LABEL_LENGTH,&_Display.data->date, frame_date, Align::CENTER);
}

void view4(void)
{
    view_new_inAllocatedPlace(&_Display.view);
    top_bar();

    auto frame = get_frame_bar();
    
    //std::cout << "frame: " << frame_to_string(frame) << std::endl;
    //std::cout << "height left: " << get_height_left(frame) << std::endl;

    Frame top{0, TOP_BAR_HEIGHT, frame.width, frame.height - (DISPLAY_HEIGHT / 10)};
    Frame top_labels{top.x,top.y,top.width, TOP_BAR_HEIGHT};
    //std::cout << "top: " << frame_to_string(top) << std::endl;
    Frame bottom{0, get_frame_top_y(top), frame.width, get_height_left(top)};
    //std::cout << "bottom: " << frame_to_string(bottom) << std::endl;

    static float min = 0.0, max = 20.0;
    {
        PlotSettings plot_sett{top, false,false,&min, &max, 
        &_Display.data->forecast.windgusts_10m.array, 
        FORECAST_SENSOR_DATA_LEN, {0xf,0x0,0x1}}; 
        Settings set;
        set.plot = plot_sett;
        Window plot_win{set, plot_float}; 
        view_addNewWindow(&_Display.view,plot_win);
    }
    {
        PlotSettings plot_sett{top, false,false,&min, &max, 
        &_Display.data->forecast.windspeed_10m.array, 
        FORECAST_SENSOR_DATA_LEN, {0,0xf,0}}; 
        Settings set;
        set.plot = plot_sett;
        Window plot_win{set, plot_float}; 
        view_addNewWindow(&_Display.view,plot_win);
    }
    {
        static float min = 0.0, max = 3.0;
        PlotSettings plot_sett{top, false,false,&min, &max, 
        &_Display.data->forecast.precipitation.array, 
        FORECAST_SENSOR_DATA_LEN, {0,0,0xf}}; 
        Settings set;
        set.plot = plot_sett;
        Window plot_win{set, plot_float}; 
        view_addNewWindow(&_Display.view,plot_win);
    }


    //add_label("|    |    |    |    |", bottom);
}

void view_charge(void)
{

}

void view_main(void)
{
    
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
