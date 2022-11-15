#ifndef INTERRUPT_H
#define INTERRUPT_H

#include "pico/stdlib.h"
#include "hardware/gpio.h"

typedef void (*interr)(void);

struct interrupt
{
    int pin;
    interr callbackFunc;
    gpio_irq_level event;
};

void interruptSetupCore0(void);
void interruptSetupCore1(void);

class Button_Interface
{
protected:
    interrupt interrupt_pressed;
    interrupt interrupt_released;
public:
    virtual void on_call_press(void);
    virtual void on_call_release(void);
    const interrupt& get_interrupt_pressed() const;
    const interrupt& get_interrupt_released() const;
};

#endif
