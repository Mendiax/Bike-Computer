
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

// interrupt button0 = {BTN0, blank, GPIO_IRQ_EDGE_FALL};
// // top right
// interrupt button1 = {BTN1, blank, GPIO_IRQ_EDGE_FALL};
// interrupt button1rel = {BTN1, blank, GPIO_IRQ_EDGE_RISE};
// // bottom right
// interrupt button2 = {BTN2, blank, GPIO_IRQ_EDGE_FALL};
// interrupt button2rel = {BTN2, blank, GPIO_IRQ_EDGE_RISE};

// interrupt button3 = {BTN3, blank, GPIO_IRQ_EDGE_FALL};

static void blank()
{

}

void Button::on_call_press(void)
{
    uint32_t current_time = to_ms_since_boot(get_absolute_time());
    //printf("btn pressed\n");
    if(current_time - this->time_press < PRESS_TIMEOUT)
    {
        // too fast press -> error or unwanted press
        return;
    }
    this->time_press = current_time;

    // run callback if exist
    this->on_press();
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
    //printf("btn released\n");

    if(this->pressed)
    {
        uint32_t press_time = this->time_release - this->time_press;
        if(press_time >= LONG_PRESS_MS)
        {
            this->was_pressed_long = true;
            this->long_call();
        }
        else if(press_time > 0)
        {
            this->was_pressed = true;
            this->short_call();
        }
        this->pressed = false;
    }

}


Button::Button(int pin)
{
    this->time_press = to_ms_since_boot(get_absolute_time());
    this->time_release = this->time_press;

    this->interrupt_pressed = interrupt{pin, blank, GPIO_IRQ_EDGE_FALL};
    this->interrupt_released = interrupt{pin, blank, GPIO_IRQ_EDGE_RISE};

    this->reset_callback();
}
void Button::reset_callback()
{
    this->short_call = blank;
    this->long_call = blank;
    this->on_press = blank;
}
void Button::set_callback(btn_call callback)
{
    this->short_call = callback;
}
void Button::set_callback_long(btn_call callback)
{
    this->long_call = callback;
}
bool Button::is_pressed()
{
    // absolute_time_t current_time = get_absolute_time();
    // if(absolute_time_diff_us(time_press, current_time) > 0 &&
    //    absolute_time_diff_us(time_press, time_release) < 0)
    // {
    //     pressed = true;
    // }
    // else
    // {
    //     pressed = false;
    // }
    return this->pressed;
}
bool Button::is_pressed_long()
{
    uint32_t current_time = to_ms_since_boot(get_absolute_time());
    return this->pressed && (current_time - time_press >= LONG_PRESS_MS);
}
bool Button::is_pressed_execute()
{
    bool is_pressed = this->pressed;
    if(is_pressed)
    {
        this->pressed = false;
    }
    return is_pressed;
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
bool Button::is_released()
{
    return (this->pressed == false);
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