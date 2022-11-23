#ifndef CORE0_H
#define CORE0_H
#include "common_actors.hpp"
#include "common_data.hpp"

class Data_Actor : public Actor
{
public:
    struct Sig_Data_Actor_Get_File_Respond {
        actors_common::File_Respond type;
        std::string file_content;
    };

    struct Sig_Data_Actor_Load_Session {
        std::string session_string;
    };

    struct Sig_Data_Actor_Set_Config {
        std::string file_name;
        std::string file_content;
    };

    struct Sig_Data_Actor_Set_Total {
        float ridden_dist_total;
        float ridden_time_total;
    };

private:
    Data_Queue<actors_common::Packet>* pc_queue;

    static void handle_sig_session_load(const Signal &sig);
    static void handle_sig_session_start(const Signal &sig);
    static void handle_sig_pause(const Signal &sig);
    static void handle_sig_start(const Signal &sig);
    static void handle_sig_stop(const Signal &sig);
    static void handle_sig_continue(const Signal &sig);

    static void handle_sig_set_config(const Signal &sig);
    static void handle_sig_set_total(const Signal &sig);
    static void handle_sig_get_file_respond(const Signal &sig);

    void handler_setup()
    {
        this->handler_add(handle_sig_pause, actors_common::SIG_DATA_ACTOR_PAUSE);
        this->handler_add(handle_sig_continue, actors_common::SIG_DATA_ACTOR_CONTINUE);
        this->handler_add(handle_sig_start, actors_common::SIG_DATA_ACTOR_START);
        this->handler_add(handle_sig_stop, actors_common::SIG_DATA_ACTOR_STOP);
        this->handler_add(handle_sig_session_start, actors_common::SIG_DATA_ACTOR_SESSION_START);
        this->handler_add(handle_sig_session_load, actors_common::SIG_DATA_ACTOR_SESSION_LOAD);
        this->handler_add(handle_sig_set_config, actors_common::SIG_DATA_ACTOR_SET_CONFIG);
        this->handler_add(handle_sig_set_total, actors_common::SIG_DATA_ACTOR_SET_TOTAL);
        this->handler_add(handle_sig_get_file_respond, actors_common::SIG_DATA_ACTOR_GET_FILE_RESPOND);
    }

    inline void set_pc_queue(Data_Queue<actors_common::Packet>* pc_queue)
    {
        this->pc_queue = pc_queue;
    }
    void setup(void);
    int loop(void);
    int loop_frame_update();
    void run_thread(void);
    Data_Actor()
    {
        handler_setup();
    }
    friend void core_setup(void);
public:
    static inline Data_Actor& get_instance()
    {
        static Data_Actor* this_p = nullptr;
        if(this_p == nullptr)
        {
            this_p = new Data_Actor();
        }

        return *this_p;
    }
};

#endif