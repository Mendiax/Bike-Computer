#include "bc_test.h"
#include "actors.hpp"

#include <pico/multicore.h>

#include <atomic>


enum Sig_Ids
{
    SIG_NOT_EXIST,
    SIG_ID_REQ_P,
    SIG_ID_IND_P
};

struct Sig_Id_Ind
{
    int id;
};

struct Sig_Id_Req
{};

int last_id = 0;

class Ind_Actor : public Actor
{
private:
    static void handle_id_ind(const Signal &sig)
    {
        Sig_Id_Ind* ind_p = sig.get_payload<Sig_Id_Ind*>();
        last_id = ind_p->id;
        delete ind_p;
        // printf("ind received\n");
    }
    void handler_setup()
    {
        //printf("override!\n");
        this->handlers.emplace_back(handle_id_ind, SIG_ID_IND_P);
    }
public:
    Ind_Actor()
    {
        handler_setup();
    }
};

int counter = 0;

class Req_Actor : public Actor
{
private:
    //static int counter;
    static void handle_id_req(const Signal &sig)
    {
        // printf("req received\n");
        Actor* actor_p = (Actor*)sig.get_actor();
        auto req_p = sig.get_payload<Sig_Id_Req*>();
        delete req_p;

        Sig_Id_Ind* payload_p = new Sig_Id_Ind();
        payload_p->id = ++counter;

        Signal ind_p(SIG_ID_IND_P, payload_p);
        actor_p->send_signal(ind_p);
}

    void handler_setup()
    {
        //printf("override!\n");
        this->handlers.emplace_back(handle_id_req, SIG_ID_REQ_P);
    }
public:
    Req_Actor()
    {
        handler_setup();
    }

};

Req_Actor req_actor;
Ind_Actor ind_actor;

// bool Ind_Actor::handle_id_ind(const Signal &sig)


// bool Req_Actor::handle_id_req(const Signal &sig)



int multicore_actor();


volatile std::atomic_bool end_core1 = false;

static void multicore_display_actor()
{
    while (end_core1 != true)
    {
        req_actor.handle_next_signal();
    }
}


int multicore_actor()
{
    printf("multicore start\n");
    {
        // reset
        last_id = 0;
        counter = 0;
        // create signal
        auto payload = new Sig_Id_Req();
        Signal request(SIG_ID_REQ_P, payload);
        // send it
        req_actor.send_signal(request, &ind_actor);

        // sim multithreading
        for(int i = 0; i < 10; i++)
        {
            ind_actor.handle_next_signal();
            req_actor.handle_next_signal();
        }
        BC_CHECK(last_id == 1);
    }

    {
        // reset
        last_id = 0;
        counter = 0;
        multicore_launch_core1(multicore_display_actor);
        sleep_ms(100);

        for(int i = 0; i < 10; i++)
        {
            ind_actor.handle_next_signal();
            auto payload = new Sig_Id_Req();
            Signal request(SIG_ID_REQ_P, payload);
            // send it
            req_actor.send_signal(request, &ind_actor);
        }

        while (!ind_actor.is_queue_empty())
        {
            sleep_ms(10);
            ind_actor.handle_next_signal();
        }
        BC_CHECK(last_id == 10);

        end_core1 = true;
        sleep_ms(100);
        multicore_reset_core1();
    }

    return 0;
}