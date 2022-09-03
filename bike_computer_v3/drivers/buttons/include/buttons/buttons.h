#ifndef _BUTTONS_H
#define _BUTTONS_H

#include <interrupts/interrupts.hpp>

#define BTN0 15
#define BTN1 21
#define BTN2 2 // speed emulation
#define BTN3 3

// extern interrupt button0;
// extern interrupt button1;
// extern interrupt button1rel;
// extern interrupt button2;
// extern interrupt button2rel;
// extern interrupt button3;


#define LONG_PRESS_MS 500
#define PRESS_TIMEOUT 200

typedef void (*btn_call)(void);

class Button : public Button_Interface
{
private:
    volatile bool pressed;

    // TODO volatile
    absolute_time_t time_press;
    absolute_time_t time_release;

    btn_call short_call;
    btn_call long_call;
    btn_call on_press;



public:
    void on_call_press(void) override;
    void on_call_release(void) override;
    Button(int pin);
    void reset_callback();
    void set_callback(btn_call callback);
    void set_callback_long(btn_call callback);
    bool is_pressed();
    bool is_pressed_long();
    /**
     * @brief check if is pressed if so set press flag to true
     *
     * @return true
     * @return false
     */
    bool is_pressed_execute();
    /**
     * @brief check if is pressed long if so set press flag to true
     *
     * @return true
     * @return false
     */
    bool is_pressed_long_execute();
    bool is_released();
    // const interrupt& get_interrupt_pressed();
    // const interrupt& get_interrupt_released();
};

extern Button btn0;
extern Button btn1;
extern Button btn2;
extern Button btn3;

static inline void buttons_clear_all()
{
    btn0.reset_callback();
    btn1.reset_callback();
    btn2.reset_callback();
    btn3.reset_callback();
}

#endif
