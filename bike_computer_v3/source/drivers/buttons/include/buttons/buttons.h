#ifndef _BUTTONS_H
#define _BUTTONS_H

#include <interrupts/interrupts.hpp>

#define BTN0 15
#define BTN1 21
#define BTN2 2
#define BTN3 3


#define LONG_PRESS_MS 1000
#define PRESS_TIMEOUT 500

typedef void (*btn_call)(void);

class Button : public Button_Interface
{
private:
    volatile bool pressed;

    volatile uint32_t time_press;
    volatile uint32_t time_release;

    volatile bool was_pressed;
    volatile bool was_pressed_long;

public:
    void on_call_press(void) override;
    void on_call_release(void) override;
    Button(unsigned pin);

    /**
     * @brief check if is pressed long if so set press flag to true
     *
     * @return true
     * @return false
     */
    bool is_pressed_long_execute();
    /**
     * @brief used for executing code outside of interrupt
     *
     * @return true
     * @return false
     */
    bool pop_was_pressed();
    /**
     * @brief used for executing code outside of interrupt
     *
     * @return true
     * @return false
     */
    bool pop_was_pressed_long();
};



extern Button btn0;
extern Button btn1;
extern Button btn2;
extern Button btn3;

#endif
