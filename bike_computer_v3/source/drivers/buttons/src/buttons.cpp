
#include "buttons/buttons.h"
#include <stdio.h>


// static declarations

//default function for interrupts
static void blank();

// global definitions
Button btn0(BTN0);
Button btn1(BTN1);
Button btn2(BTN2);
Button btn3(BTN3);

static void blank()
{

}

void Button::on_call_press(void)
{
    uint32_t current_time = to_ms_since_boot(get_absolute_time());
    if(current_time - this->time_press < PRESS_TIMEOUT)
    {
        // too fast press -> error or unwanted press
        return;
    }
    this->time_press = current_time;

    // run callback if exist
    this->pressed = true;
}

void Button::on_call_release(void)
{
    uint32_t current_time = to_ms_since_boot(get_absolute_time());
    if(current_time - this->time_release < PRESS_TIMEOUT)
    {
        // too fast press -> error or unwanted press
        return;
    }
    this->time_release = current_time;

    if(this->pressed)
    {
        uint32_t press_time = this->time_release - this->time_press;
        if(press_time >= LONG_PRESS_MS)
        {
            this->was_pressed_long = true;
        }
        else if(press_time > 0)
        {
            this->was_pressed = true;
        }
        this->pressed = false;
    }
}


Button::Button(unsigned pin)
{
    this->time_press = to_ms_since_boot(get_absolute_time());
    this->time_release = this->time_press;

    this->interrupt_pressed = Interrupt{pin, blank, GPIO_IRQ_EDGE_FALL};
    this->interrupt_released = Interrupt{pin, blank, GPIO_IRQ_EDGE_RISE};

}
bool Button::is_pressed_long_execute()
{
    uint32_t current_time = to_ms_since_boot(get_absolute_time());
    bool is_pressed = this->pressed &&
                      (current_time - time_press >= LONG_PRESS_MS);
    if(is_pressed)
    {
        this->pressed = false;
    }
    return is_pressed;
}

bool Button::pop_was_pressed()
{
    const bool pressed = this->was_pressed;
    this->was_pressed = false;
    return pressed;
}
bool Button::pop_was_pressed_long()
{
    const bool pressed = this->was_pressed_long;
    this->was_pressed_long = false;
    return pressed;
}
