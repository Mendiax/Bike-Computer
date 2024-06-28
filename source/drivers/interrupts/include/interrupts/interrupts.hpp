#ifndef INTERRUPT_H
#define INTERRUPT_H

#include "pico/stdlib.h"

#ifdef BUILD_MOCK_INTERRUPTS
enum gpio_irq_level {
    GPIO_IRQ_LEVEL_LOW = 0x1u,
    GPIO_IRQ_LEVEL_HIGH = 0x2u,
    GPIO_IRQ_EDGE_FALL = 0x4u,
    GPIO_IRQ_EDGE_RISE = 0x8u,
};
#else
#include "hardware/gpio.h"
#endif

typedef void (*interr)(void);

struct Interrupt
{
    unsigned pin;
    interr callbackFunc;
    gpio_irq_level event;
};

enum INTERRUPT_CORE{
    NONE,
    CORE_0,
    CORE_1
};

void interrupt_add(Interrupt interrupt, INTERRUPT_CORE core, bool pullup);

void interruptSetupCore0(void);
void interruptSetupCore1(void);

class Button_Interface
{
protected:
    Interrupt interrupt_pressed;
    Interrupt interrupt_released;
public:
    virtual void on_call_press(void) = 0;
    virtual void on_call_release(void) = 0;
    const Interrupt& get_interrupt_pressed() const;
    const Interrupt& get_interrupt_released() const;
};

#endif
