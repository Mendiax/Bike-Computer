#include "views/screenfunc/common.h"
#include "views/screenfunc/frame.h"


void render_frame(const void *settings)
{
    const FrameSettings& settings_frame = *((FrameSettings*) settings);
    const Frame& frame = settings_frame.frame;
    const display::DisplayColor& color = settings_frame.color;

    display::draw_line(frame.x, frame.y, frame.get_max_x() - 1,frame.y, color);
    display::draw_line(frame.get_max_x() - 1, frame.y, frame.get_max_x() - 1,frame.get_max_y() - 1, color);
    display::draw_line(frame.x, frame.get_max_y() -1 , frame.get_max_x() - 1,frame.get_max_y() - 1, color);
    display::draw_line(frame.x, frame.y, frame.x, frame.get_max_y() - 1, color);
}
