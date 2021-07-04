#include "bc_test.h"
#include "actors.hpp"


enum Sig_Ids
{
    SIG_PRINT_P,
    SIG_GET_ID,
    SIG_NOT_EXIST
};

struct Sig_Print
{
    int d;
    bool printed;
};

#define EXPECTED_ID 23

class Printer : public Actor
{
private:
    static void print_sig_get_id(const Signal &sig)
    {
        printf("sig handled get id\n");
        Sig_Print *print_p = sig.get_payload<Sig_Print*>();
        print_p->d = EXPECTED_ID;
    }
    static void print_sig_0(const Signal &sig)
    {
        printf("sig handled print\n");
        Sig_Print *print_p = sig.get_payload<Sig_Print*>();
        print_p->printed = true;
    }
    void handler_setup()
    {
        //printf("override!\n");
        this->handlers.emplace_back(print_sig_get_id, SIG_GET_ID);
        this->handlers.emplace_back(print_sig_0, SIG_PRINT_P);
    }
public:
    Printer()
    {
        handler_setup();
    }
};

static int simple_actor();
extern int multicore_actor();


static int simple_actor()
{
    Printer printer_actor;
    //printer_actor.handler_setup();

    // create signal
    Sig_Print payload{0, false};
    Signal test_print(SIG_PRINT_P, &payload);
    Signal test_id(SIG_GET_ID, &payload);


    Sig_Print *print_p = test_print.get_payload<Sig_Print*>();
    BC_CHECK(print_p == &payload);
    BC_CHECK(print_p->d == payload.d);

    // send it
    printer_actor.send_signal(test_print);
    printer_actor.send_signal(test_id);


    while (!printer_actor.is_queue_empty())
    {
        printer_actor.handle_next_signal();
    }
    BC_CHECK(payload.printed == true);
    BC_CHECK(payload.d == EXPECTED_ID);

    printf("simple test passed\n");


    return 0;
}

int test_actors(void)
{
    return simple_actor() || multicore_actor();
}