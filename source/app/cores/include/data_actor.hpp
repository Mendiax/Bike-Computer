#ifndef CORE0_H
#define CORE0_H

// #-------------------------------#
// |           includes            |
// #-------------------------------#

// c/c++ includes

// my includes
#include "common_actors.hpp"

// #-------------------------------#
// |            macros             |
// #-------------------------------#

// #-------------------------------#
// | global types declarations     |
// #-------------------------------#

/**
 * @brief Data_Actor class responsible for handling various signals related to data processing.
 */
class Data_Actor : public Actor
{
public:
    /**
     * @brief Signal structure for loading session data.
     */
    struct Sig_Data_Actor_Load_Session {
        std::string session_string; ///< The session string to load.
    };

    /**
     * @brief Signal structure for setting configuration.
     */
    struct Sig_Data_Actor_Set_Config {
        std::string file_name;     ///< The name of the configuration file.
        std::string file_content;  ///< The content of the configuration file.
    };

    /**
     * @brief Signal structure for setting total ridden distance and time.
     */
    struct Sig_Data_Actor_Set_Total {
        float ridden_dist_total;  ///< Total ridden distance.
        float ridden_time_total;  ///< Total ridden time.
    };

    /**
     * @brief Signal structure for requesting a display packet.
     */
    struct Sig_Display_Actor_Req_Packet {
        actors_common::Packet* packet_p; ///< Pointer to the packet.
    };

private:
    Gear_Suggestion_Calculator* gear_suggestion_calc; ///< Pointer to gear suggestion calculator.

    static void handle_sig_session_load(const Signal &sig);
    static void handle_sig_session_start(const Signal &sig);
    static void handle_sig_pause(const Signal &sig);
    static void handle_sig_start(const Signal &sig);
    static void handle_sig_stop(const Signal &sig);
    static void handle_sig_continue(const Signal &sig);
    static void handle_sig_set_config(const Signal &sig);
    static void handle_sig_set_total(const Signal &sig);
    static void handle_sig_req_packet(const Signal &sig);

    /**
     * @brief Sets up signal handlers for the Data_Actor.
     */
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
        this->handler_add(handle_sig_req_packet, actors_common::SIG_DATA_ACTOR_REQ_PACKET);
    }

    /**
     * @brief Runs the main thread for the Data_Actor.
     */
    void run_thread(void);

    /**
     * @brief Sets up the Data_Actor.
     */
    void setup(void);

    /**
     * @brief Main loop for the Data_Actor.
     * @return An integer indicating the loop result.
     */
    int loop(void);

    /**
     * @brief Updates the frame in the main loop.
     * @return An integer indicating the frame update result.
     */
    int loop_frame_update();

    friend void core_setup(void);

    /**
     * @brief Constructor for Data_Actor, initializes signal handlers.
     */
    Data_Actor()
    {
        handler_setup();
    }

public:
    /**
     * @brief Provides a singleton instance of Data_Actor.
     * @return Reference to the singleton instance.
     */
    static inline Data_Actor& get_instance()
    {
        static Data_Actor* this_p = nullptr;
        if (this_p == nullptr)
        {
            this_p = new Data_Actor();
        }

        return *this_p;
    }
};

#endif // CORE0_H
