#ifndef __WINDOW_H__
#define __WINDOW_H__

#include "../addons/print.h"
#include <assert.h>
#include "screen.h"

/* data ptr, additional data ptr, window(size) */
typedef void(*drawFunc_p)(void*,void*,Frame*);

/* class containg data to draw on window */
class Window
{
private:
    Frame frame;
    void* data;
    void* settings;
    drawFunc_p updateFunc_p;

public:
    Window(Frame newFrame, void* dataSource, void* drawSettings, drawFunc_p drawFunction)
    {
        this->frame = newFrame;
        this->data = dataSource;
        this->settings = drawSettings;
        this->updateFunc_p = drawFunction;

        //check if frame has good parameters
        //PRINT_DEBUG("checking frame dim");
        assert(frame.x >= 0 && frame.x <= SCREEN_WIDTH);
        assert(frame.width >= 0 && frame.width <= SCREEN_WIDTH);
        assert(frame.y >= 0 && frame.y <= SCREEN_HEIGHT);
        assert(frame.height >= 0 && frame.height <= SCREEN_HEIGHT);
        //PRINT_DEBUG("frame ok");
    }
    ~Window();

    void update(){
        TRACE_FRAME_PRINT("Frame: " + String(frame.x) + "," + String(frame.y) + " update");
        updateFunc_p(data, settings, &frame);
    }
};

#endif
