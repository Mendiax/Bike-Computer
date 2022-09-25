
// #------------------------------#
// |          includes            |
// #------------------------------#
// pico includes

// c/c++ includes
#include <tuple>
#include <typeinfo>
#include <inttypes.h>

// my includes
#include "massert.h"
#include "traces.h"

#include "views/view.hpp"
#include "views/display.h"
#include "views/screenfunc/plot.h"
#include "views/screenfunc/lastval.h"
#include "views/screenfunc/label.h"
#include "views/screenfunc/val.h"

// #------------------------------#
// |           macros             |
// #------------------------------#

// #------------------------------#
// | global variables definitions |
// #------------------------------#

// #------------------------------#
// | static variables definitions |
// #------------------------------#
View_Creator* View_Creator::view_p{nullptr};

// #------------------------------#
// | static functions declarations|
// #------------------------------#

// #------------------------------#
// | global function definitions  |
// #------------------------------#
void View_Creator::reset()
{
    this->currnetNumberOfWindows = 0;
}

void View_Creator::draw()
{
    for (uint_fast8_t i = 0; i < this->currnetNumberOfWindows; i++)
    {
        TRACE_DEBUG(5, TRACE_VIEWS, "Drawing window %" PRIuFAST8 "\n", i);
        Window_update(&this->windows[i]);
        TRACE_DEBUG(6, TRACE_VIEWS, "window drawed %" PRIuFAST8 "\n", i);
    }
}

void View_Creator::add_new_window(const Window& window)
{
    TRACE_DEBUG(7, TRACE_VIEWS, "adding new window with id=%" PRIuFAST8 " \n", this->currnetNumberOfWindows);
    massert(this->currnetNumberOfWindows < MAX_NUMBER_OF_WINDOWS, "maximum numbers of windows reached in current view\n");
    this->windows[this->currnetNumberOfWindows] = window;
    this->currnetNumberOfWindows++;
}

Window* View_Creator::get_previous_window()
{
    massert(this->currnetNumberOfWindows > 0, "cannot get previous window\n");
    return &this->windows[this->currnetNumberOfWindows-1];
}







void View_Creator::add_label(const char* string, const Frame& frame, Align align, size_t commonLength )
{
    Window new_window;
    new_window.updateFunc_p = LabelDraw;
    LabelSettings& valSettings = new_window.settings.label;
    if(commonLength > 0)
    {
        labelSettingsNew(valSettings, frame, string, commonLength);
    }
    else
    {
        labelSettingsNew(valSettings, frame, string);
    }

    labelSettingsAlign(valSettings, frame, align);
    add_new_window(new_window);

}

void View_Creator::add_Vertical(const char* over, const char* under, const Frame& frame, Align align)
{
    const uint_fast16_t str_len_max = std::max(strlen(over), strlen(under));
    const uint_fast16_t half_height = frame.height >> 1;
    const Frame unitTop = {frame.x, frame.y, frame.width, (uint16_t)half_height};
    add_label(over, unitTop,  align, str_len_max);

    const uint16_t top_unit_max_y = frame.y + labelSettingsGetHeight(get_previous_window()->settings.label);
    const Frame unitBottom = {frame.x, top_unit_max_y,
                              frame.width, (uint16_t)half_height};
    add_label(under, unitBottom,  align, str_len_max);
}

void View_Creator::add_Units(const char* over, const char* under, const Frame& frame, Align align = Align::LEFT)
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
std::tuple<Frame, Frame> View_Creator::splitFrame(const Frame& frame, uint16_t length, bool align_right)
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
constexpr std::tuple<Frame, Frame> View_Creator::split_vertical(const Frame& frame)
{
    const uint16_t half_width = frame.width / 2;
    const Frame f1 = {frame.x, frame.y, half_width, frame.height};
    const uint16_t next_x = frame.x + half_width;
    const Frame f2 = {next_x, frame.y, half_width, frame.height};
    return std::make_tuple(f1,f2);
}
/**
 * @brief splits frame into 2 frames with half of height
 *
 * @param frame
 * @return constexpr auto
 */
constexpr std::tuple<Frame, Frame> View_Creator::split_horizontal(const Frame& frame)
{
    const uint16_t half_height = frame.height / 2;
    const Frame f1 = {frame.x, frame.y, frame.width, half_height};
    const uint16_t next_y = frame.y + half_height;
    const Frame f2 = {frame.x, next_y, frame.width, half_height};

    return std::make_tuple(f1,f2);
}

void View_Creator::top_bar(const Time_HourS *hours, const Battery *lipo)
{
    Frame bar_frame = {0, 0, DISPLAY_WIDTH, TOP_BAR_HEIGHT};
    auto [frame_hour, frame_battery] = split_vertical(bar_frame);
    add_value("/\\%3d" PRIu8, BAT_LEVEL_LABEL_LENGTH, lipo, frame_battery, Align::RIGHT);
    add_value("", TIMES_LABEL_LENGTH, hours, frame_hour, Align::LEFT);
}

constexpr Frame View_Creator::get_frame_bar()
{
    return {0, TOP_BAR_HEIGHT, DISPLAY_WIDTH, DISPLAY_HEIGHT - TOP_BAR_HEIGHT};
}

Frame View_Creator::setup_bar(const Time_HourS* hours, const Battery* lipo)
{
    top_bar(hours, lipo);
    return get_frame_bar();
}

// static inline uint16_t View_Creator::get_frame_top_y(const Frame frame)
// {
//     massert(DISPLAY_HEIGHT >= frame.y + frame.height, "no space left Y:%" PRIu16  " H:%" PRIu16 "\n", frame.y, frame.height);
//     return frame.y + frame.height;
// }

// static inline uint16_t View_Creator::get_height_left(const Frame frame)
// {
//     massert(DISPLAY_HEIGHT >= get_frame_top_y(frame), "no space left\n");
//     return DISPLAY_HEIGHT - get_frame_top_y(frame);
// }


// #------------------------------#
// | static functions definitions |
// #------------------------------#