#ifndef INTERRUPT_H
#define INTERRUPT_H

#include "pico/stdlib.h"
#include "hardware/gpio.h"

typedef void (*interr)(void);

struct Interrupt
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
    Interrupt interrupt_pressed;
    Interrupt interrupt_released;
public:
    virtual void on_call_press(void);
    virtual void on_call_release(void);
    const Interrupt& get_interrupt_pressed() const;
    const Interrupt& get_interrupt_released() const;
};

#endif
