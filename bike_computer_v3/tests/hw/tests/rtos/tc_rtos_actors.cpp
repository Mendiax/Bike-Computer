#include "pico_test.hpp"
#include "pico/stdlib.h"
#include <cstdint>
#include <stdio.h>

#include "signals.hpp"
#include "traces.h"
#include "actors.hpp"


#include <FreeRTOS.h>
#include <task.h>

#define SIG_ADD 0
#define SIG_RES 1

#define SEND_VAL 10

// TEST_CASE_SUB_FUNCTION_DEF(adder_test)
// {
//     return;
// }

// PICO_TEST_VARS
static TestCaseStats __stats;
#define __CONTEXT "Actor_A"

class Actor_A : public Actor
{
    private:
    static void handle_sig_inc_x(const Signal &sig)
    {
        Actor* sender = (Actor*)sig.get_actor();

        auto val = sig.get_payload<int>();
        val += 1;
        sender->send_signal(Signal(SIG_RES, &val));
    }

    static void handle_sig_inc_x_res(const Signal &sig)
    {
        auto val = sig.get_payload<int>();
        PRINT("received " << val);
        PICO_TEST_ASSERT(val == (SEND_VAL + 1));
    }

    void handler_setup()
    {
        this->handler_add(handle_sig_inc_x, SIG_ADD);
        this->handler_add(handle_sig_inc_x_res, SIG_RES);
    }
public:
    Actor_A()
    {
        handler_setup();
    }

};
#undef __CONTEXT

Actor_A* actor_0;
Actor_A* actor_1;


void task_actor_0(void* p)
{
    (void)p;
    int val = SEND_VAL;
    actor_1->send_signal(Signal(SIG_ADD, &val, actor_0));

    while (1) {
        actor_0->handle_all();
        vTaskDelay(100);
    }

}

void task_actor_1(void* p)
{
    (void)p;

    while (1) {
        actor_1->handle_all();
        vTaskDelay(100);
    }
}


TEST_CASE_FUNCTION(tc_rtos_actors)
{
    actor_0 = new Actor_A();
    actor_1 = new Actor_A();


    ::__stats = __stats;
    // TEST_CASE_SUB_FUNCTION(adder_test);

}
