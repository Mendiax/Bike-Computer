#ifndef _BUTTONS_H
#define _BUTTONS_H

// #-------------------------------#
// |           includes            |
// #-------------------------------#

// c/c++ includes

// my includes
#include <interrupts/interrupts.hpp>

// #-------------------------------#
// |            macros             |
// #-------------------------------#

#define BTN0 15
#define BTN1 21
#define BTN2 2
#define BTN3 3

#define LONG_PRESS_MS 1000
#define PRESS_TIMEOUT 500

// #-------------------------------#
// | global types declarations     |
// #-------------------------------#

typedef void (*btn_call)(void);

/**
 * @brief Button class for handling button press and release events.
 */
class Button : public Button_Interface
{
private:
    volatile bool pressed;                 ///< Flag indicating if the button is currently pressed.
    volatile uint32_t time_press;          ///< Timestamp of the button press event.
    volatile uint32_t time_release;        ///< Timestamp of the button release event.
    volatile bool was_pressed;             ///< Flag indicating if the button was pressed.
    volatile bool was_pressed_long;        ///< Flag indicating if the button was pressed long.

public:
    /**
     * @brief Constructor for the Button class.
     * @param pin The pin number to which the button is connected.
     */
    Button(unsigned pin);

    /**
     * @brief Override function called on button press.
     */
    void on_call_press(void) override;

    /**
     * @brief Override function called on button release.
     */
    void on_call_release(void) override;

    /**
     * @brief Checks if the button was pressed long enough to be considered a long press.
     * If so, sets the long press flag to true.
     * @return True if the button was pressed long, false otherwise.
     */
    bool is_pressed_long_execute();

    /**
     * @brief Checks and resets the flag indicating if the button was pressed.
     * Used for executing code outside of the interrupt.
     * @return True if the button was pressed, false otherwise.
     */
    bool pop_was_pressed();

    /**
     * @brief Checks and resets the flag indicating if the button was pressed long.
     * Used for executing code outside of the interrupt.
     * @return True if the button was pressed long, false otherwise.
     */
    bool pop_was_pressed_long();
};

// #-------------------------------#
// | global variables declarations |
// #-------------------------------#

extern Button btn0;
extern Button btn1;
extern Button btn2;
extern Button btn3;

#endif // _BUTTONS_H
