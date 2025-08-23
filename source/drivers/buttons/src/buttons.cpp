
#include "buttons/buttons.h"
#include <stdio.h>
#include "traces.h"

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
    const auto current_time = get_absolute_time();

    if(this->pressed || absolute_time_diff_us(this->time_press, current_time) < PRESS_TIMEOUT)
    {
        // too fast press -> error or unwanted press
        return;
    }
    TRACE_DEBUG(1, TRACE_BUTTON, "[%d] Button pressed\n", this->pin);
    this->time_press = current_time;

    // run callback if exist
    this->pressed = true;
}

void Button::on_call_release(void)
{
    const auto current_time = get_absolute_time();
    if(!this->pressed || absolute_time_diff_us(this->time_release, current_time) < PRESS_TIMEOUT)
    {
        // too fast press -> error or unwanted press
        return;
    }
    this->time_release = current_time;

    const auto press_time = absolute_time_diff_us(this->time_press, this->time_release);
    TRACE_DEBUG(1, TRACE_BUTTON, "[%d] Button released %ld (%d)\n", this->pin, press_time, press_time >= LONG_PRESS_US);

    if(press_time >= LONG_PRESS_US)
    {
        this->was_pressed_long = true;
    }
    else if(press_time > 0)
    {
        this->was_pressed = true;
    }
    this->pressed = false;
}


Button::Button(unsigned pin)
{
    this->pin = pin;
    this->pressed = false;
    this->was_pressed = false;
    this->was_pressed_long = false;
    this->time_press = to_ms_since_boot(get_absolute_time());
    this->time_release = this->time_press;

    this->interrupt_pressed = Interrupt{pin, blank, GPIO_IRQ_EDGE_FALL};
    this->interrupt_released = Interrupt{pin, blank, GPIO_IRQ_EDGE_RISE};

}
bool Button::is_pressed_long_execute()
{
    const bool is_pressed = this->pressed &&
                      (absolute_time_diff_us(time_press, get_absolute_time()) >= LONG_PRESS_US);
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
