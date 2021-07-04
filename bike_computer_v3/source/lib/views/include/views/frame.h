#ifndef FRAME_H
#define FRAME_H

#include <stdint.h>
#include <string>

typedef struct Frame
{
    uint16_t x, y, width, height;

    inline uint16_t get_max_x() const
    {
        return x + width;
    }
    inline uint16_t get_max_y() const
    {
        return y + height;
    }
} Frame;

static inline Frame offset_x(const Frame& frame, int16_t offset)
{
    Frame offset_frame = frame;
    int16_t new_x = (int16_t)frame.x + offset;
    if(new_x < 0){new_x = 0;}
    offset_frame.x = new_x;
    return offset_frame;
}

static inline Frame offset_y(const Frame& frame, int16_t offset)
{
    Frame offset_frame = frame;
    int16_t new_y = (int16_t)frame.y + offset;
    if(new_y < 0){new_y = 0;}
    offset_frame.y = new_y;
    return offset_frame;
}

static inline std::string frame_to_string(const Frame& frame)
{

    return " [" + std::to_string(frame.x) + "," + std::to_string(frame.x + frame.width) + "], " +
           " [" + std::to_string(frame.y) + "," + std::to_string(frame.y + frame.height) + "], ";
}

#endif
