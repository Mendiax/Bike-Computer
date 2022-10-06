#ifndef __VIEW_CREATOR_H__
#define __VIEW_CREATOR_H__

// #-------------------------------#
// |           includes            |
// #-------------------------------#
// pico includes

// c/c++ includes
#include <stdint.h>
#include <tuple>
#include <array>
#include <string.h>
#include <sstream>

// my includes
#include "views/frame.h"
#include "views/screenfunc/common.h"
#include "views/screenfunc/val.h"
#include "views/screenfunc/label.h"


#include "window.h"
#include "massert.hpp"

// #-------------------------------#
// |            macros             |
// #-------------------------------#
#define TOP_BAR_HEIGHT (DISPLAY_HEIGHT / 10)
#define MAX_NUMBER_OF_WINDOWS 13

#define VIEW_DEBUG

#ifdef VIEW_DEBUG
    #define VIEW_DEBUG_ADD(...) __VA_ARGS__
#else
    #define VIEW_DEBUG_ADD(...)
#endif

#define MAX_LABEL_ARR_LEN 32

// #-------------------------------#
// | global types declarations     |
// #-------------------------------#

/**
 * @brief class setting up one view for display contaninig multiple windows
 *
 */
class View_Creator
{
private:
    uint8_t currnetNumberOfWindows;
    Window windows[MAX_NUMBER_OF_WINDOWS];
protected:
    static View_Creator* view_p;
    View_Creator(){};
public:
    // remove not wanted constructors
    View_Creator(View_Creator &other) = delete;
    void operator=(const View_Creator &) = delete;

    static inline View_Creator* get_view()
    {
        if(view_p == nullptr)
        {
            view_p = new View_Creator();
        }
        return view_p;
    }
    void reset();
    void draw();
    void add_new_window(const Window& window);
    Window* get_previous_window();

    void add_frame(const Frame& frame, const display::DisplayColor& color = FONT_FOREGROUND);
    void add_label(const char* string, const Frame& frame, Align align = Align::LEFT, size_t commonLength = 0, const display::DisplayColor& color = FONT_FOREGROUND);
    template<typename T>
    void add_value(const char* format, size_t commonLength, const T* data, const Frame& frame, Align align = Align::LEFT);
    void add_Vertical(const char* over, const char* under, const Frame& frame, Align align = Align::LEFT);
    template <typename T, typename Q>
    void add_Vertical(const char *overFormat, size_t overCommonLength, const T *overData,
                      const char *underFormat, size_t underCommonLength, const Q *underData,
                      const Frame &frame, Align align = Align::LEFT);
    void add_Units(const char* over, const char* under, const Frame& frame, Align align);

    /**
     * @brief adds label in fomrat:
     * |<x0> <x1> ... <xn>| evenly distributed
     *
     * @tparam T
     * @tparam N
     * @param frame
     */
    template<typename T>
    uint16_t add_arr_label(const T* arr, size_t N, const Frame& frame);

    /**
     * @brief split frame into fram with len-1/len width and 1/len width
     *
     * @param frame
     * @param len
     * @return std::tuple<Frame, Frame>
     */
    static std::tuple<Frame, Frame> splitFrame(const Frame &frame, uint16_t length, bool align_right = false);

    /**
     * @brief splits frame into 2 frames with half of width
     *
     * @param frame
     * @return constexpr std::tuple<Frame, Frame>
     */
    static std::tuple<Frame, Frame> split_vertical(const Frame &frame, uint8_t ratio = 2, bool invert = false);
    // std::tuple<Frame, Frame> split_vertical(const Frame& frame, float ratio);

    /**
     * @brief splits frame into 2 frames with half of height
     *
     * @param frame
     * @return constexpr std::tuple<Frame, Frame>
     */
    static std::tuple<Frame, Frame> split_horizontal(const Frame &frame, uint8_t ratio = 2, bool invert = false);

    static std::vector<Frame> split_horizontal_arr(const Frame &frame, uint8_t cnt);


    template <typename T>
    void addValueUnitsVertical(const char *format, size_t commonLength, const T *data,
                               const char *over, const char *under,
                               const Frame &frame,
                               Align align = Align::LEFT, bool alignRight = true);

    template <typename T, typename Q, typename P>
    void addValueValuesVertical(const char *format, size_t commonLength, const T *data,
                                const char *overFormat, size_t overCommonLength, const Q *overData,
                                const char *underFormat, size_t underCommonLength, const P *underData,
                                const Frame &frame,
                                Align align = Align::LEFT, bool alignRight = true);

    void top_bar(const Time_HourS *hours, const Battery *lipo);

    constexpr Frame get_frame_bar();
    /**
     * @brief Set the up top bar and return the rest of frame
     *
     * @return Frame
     */
    Frame setup_bar(const Time_HourS *hours, const Battery *lipo);

    static inline uint16_t get_frame_top_y(const Frame frame)
    {
        massert(DISPLAY_HEIGHT >= frame.y + frame.height, "no space left Y:%" PRIu16 " H:%" PRIu16 "\n", frame.y, frame.height);
        return frame.y + frame.height;
    }

    static inline uint16_t get_height_left(const Frame frame)
    {
        massert(DISPLAY_HEIGHT >= get_frame_top_y(frame), "no space left\n");
        return DISPLAY_HEIGHT - get_frame_top_y(frame);
    }
};

// #-------------------------------#
// | template functions definitions|
// #-------------------------------#

/**
 * @brief prints array with spaces between them
 *
 * @tparam T type of arr
 * @param N len of arr
 * @param arr array to display
 * @param frame
 * @return uint16_t width of generated label
 */
template<typename T>
uint16_t View_Creator::add_arr_label(const T* arr, size_t N, const Frame& frame)
{
    std::vector<std::string> strings(N);
    size_t max_len = 0;
    for(size_t i = 0; i < N; i++)
    {
        strings[i] = std::to_string(arr[i]);
        max_len = std::max((size_t)strings[i].length(), max_len);
    }
    const size_t combined_length = max_len * N;


    Window new_window;
    new_window.updateFunc_p = LabelDraw;
    LabelSettings& label_settings = new_window.settings.label;

    static char label_space[MAX_LABEL_ARR_LEN];

    // frame with "infinite" height
    const Frame frame_topless = {frame.x, frame.y, frame.width, UINT16_MAX};
    int no_spaces = 0;
    uint16_t width, height;
    do{
        // calc width with new amount of spaces
        const size_t whitespace_req = (N - 1) * ++no_spaces;
        const size_t space_req = combined_length + whitespace_req;

        // generate settings
        labelSettingsNew(label_settings, frame_topless, label_space, space_req);
        width = labelSettingsGetWidth(label_settings);
        height = labelSettingsGetHeight(label_settings);
    }while(height > frame.height);


    // generate string with data and spacess between
    std::stringstream ss(std::string(MAX_LABEL_ARR_LEN - 1, '\0'));

    // gen spces string
    auto whitespace = new char[no_spaces + 1];
    memset(whitespace, ' ', no_spaces);
    whitespace[no_spaces] = '\0';

    // add data
    ss << strings[0];
    for(size_t i = 1; i < N; i++)
    {
        ss << whitespace << strings[i];
    }
    // check length and copy
    massert(strlen(ss.str().c_str()) < MAX_LABEL_ARR_LEN, "generated array is too long %zu", strlen(ss.str().c_str()));
    strcpy(label_space, ss.str().c_str());

    new_window.updateFunc_p = LabelDraw;
    labelSettingsAlignHeight(label_settings.text, frame, false);
    add_new_window(new_window);
    return width;
}

template<typename T>
void View_Creator::add_value(const char* format, size_t commonLength, const T* data, const Frame& frame, Align align)
{
    Window new_window;
    new_window.updateFunc_p = getDrawFunc(data);
    labelSettingsNew(new_window.settings.label, frame, format, commonLength);

    labelSettingsAlign(new_window.settings.label, frame, align);
    new_window.settings.val.data = data;
    add_new_window(new_window);
}

template <typename T, typename Q>
void View_Creator::add_Vertical(const char *overFormat, size_t overCommonLength, const T *overData,
                                const char *underFormat, size_t underCommonLength, const Q *underData,
                                const Frame &frame, Align align)
{
    //const uint_fast16_t str_len_max = std::max(overCommonLength, underCommonLength);
    const uint_fast16_t half_height = frame.height >> 1;

    const Frame unitTop =     {frame.x, frame.y, frame.width, (uint16_t)half_height};
    add_value(overFormat, overCommonLength, overData, unitTop,  align);

    const uint_fast16_t top_unit_height = labelSettingsGetHeight(get_previous_window()->settings.label);
    const uint16_t unitBottom_y = frame.y + (uint16_t)top_unit_height;
    const Frame unitBottom =  {frame.x, unitBottom_y,
                         frame.width, (uint16_t)half_height};
    add_value(underFormat, underCommonLength, underData, unitBottom,  align);
}

template <typename T>
void View_Creator::addValueUnitsVertical(const char *format, size_t commonLength, const T *data,
                                         const char *over, const char *under,
                                         const Frame &frame,
                                         Align align, bool alignRight)
{
    auto [frameValue, frameUnits] = this->splitFrame(frame, commonLength + 1, alignRight);
    this->add_value(format, commonLength, data, frameValue);
    this->add_Units(over, under, frameUnits,  align);
}

template <typename T, typename Q, typename P>
void View_Creator::addValueValuesVertical(const char *format, size_t commonLength, const T *data,
                                           const char *overFormat, size_t overCommonLength, const Q *overData,
                                           const char *underFormat, size_t underCommonLength, const P *underData,
                                           const Frame &frame,
                                           Align align, bool alignRight)
{
    auto [frameValue, frameUnits] = this->splitFrame(frame, (uint16_t)commonLength + 1, alignRight);
    this->add_value(format, commonLength, data, frameValue);
    this->add_Vertical(overFormat, overCommonLength, overData,
                       underFormat, underCommonLength, underData,
                       frameUnits, align);
}

// #-------------------------------#
// | global variables declarations |
// #-------------------------------#

// #-------------------------------#
// | global function declarations  |
// #-------------------------------#

#endif