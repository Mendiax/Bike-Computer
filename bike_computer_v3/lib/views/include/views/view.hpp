#ifndef __VIEW_CREATOR_H__
#define __VIEW_CREATOR_H__

// #-------------------------------#
// |           includes            |
// #-------------------------------#
// pico includes

// c/c++ includes
#include <stdint.h>
#include <tuple>


// my includes
#include "views/frame.h"
#include "views/screenfunc/common.h"
#include "views/screenfunc/val.h"
#include "views/screenfunc/label.h"


#include "window.h"
#include "massert.h"

// #-------------------------------#
// |            macros             |
// #-------------------------------#
#define TOP_BAR_HEIGHT (DISPLAY_HEIGHT / 10)
#define MAX_NUMBER_OF_WINDOWS 12

#define VIEW_DEBUG

#ifdef VIEW_DEBUG
    #define VIEW_DEBUG_ADD(...) __VA_ARGS__
#else
    #define VIEW_DEBUG_ADD(...)
#endif

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

    void add_label(const char* string, const Frame& frame, Align align = Align::LEFT, size_t commonLength = 0);
    template<typename T>
    void add_value(const char* format, size_t commonLength, const T* data, const Frame& frame, Align align = Align::LEFT);
    void add_Vertical(const char* over, const char* under, const Frame& frame, Align align = Align::LEFT);
    template <typename T, typename Q>
    void add_Vertical(const char *overFormat, size_t overCommonLength, const T *overData,
                      const char *underFormat, size_t underCommonLength, const Q *underData,
                      const Frame &frame, Align align = Align::LEFT);
    void add_Units(const char* over, const char* under, const Frame& frame, Align align);

    // TODO make those static
    /**
     * @brief split frame into fram with len-1/len width and 1/len width
     *
     * @param frame
     * @param len
     * @return std::tuple<Frame, Frame>
     */
    std::tuple<Frame, Frame> splitFrame(const Frame &frame, uint16_t length, bool align_right = false);
    /**
     * @brief splits frame into 2 frames with half of width
     *
     * @param frame
     * @return constexpr std::tuple<Frame, Frame>
     */
    constexpr std::tuple<Frame, Frame> split_vertical(const Frame &frame);
    // {
    //     const uint16_t half_width = frame.width / 2;
    //     const Frame f1 = {frame.x, frame.y, half_width, frame.height};
    //     const uint16_t next_x = frame.x + half_width;
    //     const Frame f2 = {next_x, frame.y, half_width, frame.height};
    //     return std::make_tuple(f1,f2);
    // }
    /**
     * @brief splits frame into 2 frames with half of height
     *
     * @param frame
     * @return constexpr std::tuple<Frame, Frame>
     */
    constexpr std::tuple<Frame, Frame> split_horizontal(const Frame &frame);
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
    const Frame unitBottom =  {frame.x, frame.y + (uint16_t)top_unit_height,
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