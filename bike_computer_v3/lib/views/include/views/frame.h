#ifndef FRAME_H
#define FRAME_H

#include <stdint.h>
#include <string>

typedef struct Frame
{
    uint16_t x, y, width, height;

    inline uint16_t get_max_x()
    {
        return x + width;
    }
    inline uint16_t get_max_y()
    {
        return y + height;
    }
} Frame;

static inline std::string frame_to_string(const Frame& frame)
{

    return " [" + std::to_string(frame.x) + "," + std::to_string(frame.x + frame.width) + "], " +
           " [" + std::to_string(frame.y) + "," + std::to_string(frame.y + frame.height) + "], ";
}

#endif
