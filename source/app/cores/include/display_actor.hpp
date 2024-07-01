#ifndef CORE1_H
#define CORE1_H

// #-------------------------------#
// |           includes            |
// #-------------------------------#

// c/c++ includes

// my includes
#include "common_types.h"
#include "common_actors.hpp"
#include "session.hpp"
#include "gui/structure.hpp"

// #-------------------------------#
// |            macros             |
// #-------------------------------#

#ifndef FILE_SESSION_DATA
#define FILE_SESSION_DATA "session_log.csv"
#endif

// #-------------------------------#
// | global types declarations     |
// #-------------------------------#

/**
 * @brief Display_Actor class responsible for handling various signals related to display operations.
 */
class Display_Actor : public Actor
{
public:
    /**
     * @brief Signal structure for showing a message on the display.
     */
    struct Sig_Display_Actor_Show_Msg {
        std::string msg;               ///< The message to show.
        display::DisplayColor color;   ///< The color of the message.
        uint time_ms;                  ///< Duration to display the message in milliseconds.
    };

    /**
     * @brief Signal structure for logging data.
     */
    struct Sig_Display_Actor_Log {
        std::string header;   ///< The header for the log entry.
        std::string line;     ///< The line content for the log entry.
        std::string file_name;///< The file name where the log will be saved.
    };

    /**
     * @brief Signal structure for requesting a display packet.
     */
    struct Sig_Display_Actor_Get_Packet {
        actors_common::Packet* packet_p; ///< Pointer to the packet.
    };

    /**
     * @brief Signal structure for ending a session.
     */
    struct Sig_Display_Actor_End_Sesion {
        bool save; ///< Flag indicating whether to save the session.
    };

    /**
     * @brief Signal structure for saving a session.
     */
    struct Sig_Display_Actor_Save_Sesion {
        Session_Data session; ///< The session data to save.
    };

    /**
     * @brief Signal structure for loading a session.
     */
    struct Sig_Display_Actor_Load_Session {
        uint16_t session_id; ///< The ID of the session to load.
    };

    /**
     * @brief Signal structure for updating total ridden distance and time.
     */
    struct Sig_Display_Actor_Total_Update {
        float ridden_dist; ///< Total ridden distance.
        float ridden_time; ///< Total ridden time.
    };

private:
    Gui* gui;                     ///< Pointer to the GUI.
    actors_common::Packet local_data; ///< Local data packet.

    // Defined in core1.cpp
    static void handle_sig_total_update(const Signal &sig);
    static void handle_sig_load_session(const Signal &sig);
    static void handle_sig_save_session(const Signal &sig);
    static void handle_sig_log(const Signal &sig);
    static void handle_sig_show_msg(const Signal &sig);
    static void handle_sig_get_packet(const Signal &sig);

    /**
     * @brief Sets up signal handlers for the Display_Actor.
     */
    void handler_setup()
    {
        this->handler_add(handle_sig_total_update, actors_common::SIG_DISPLAY_ACTOR_TOTAL_UPDATE);
        this->handler_add(handle_sig_load_session, actors_common::SIG_DISPLAY_ACTOR_LOAD_SESSION);
        this->handler_add(handle_sig_save_session, actors_common::SIG_DISPLAY_ACTOR_SAVE_SESSION);
        this->handler_add(handle_sig_log, actors_common::SIG_DISPLAY_ACTOR_LOG);
        this->handler_add(handle_sig_show_msg, actors_common::SIG_DISPLAY_ACTOR_SHOW_MSG);
        this->handler_add(handle_sig_get_packet, actors_common::SIG_DISPLAY_ACTOR_GET_PACKET);
    }

    /**
     * @brief Sets up the Display_Actor.
     */
    void setup();

    /**
     * @brief Main loop for the Display_Actor.
     * @return An integer indicating the loop result.
     */
    int loop();

    /**
     * @brief Runs the main thread for the Display_Actor.
     */
    void run_thread(void);

    /**
     * @brief Constructor for Display_Actor, initializes signal handlers and local data.
     */
    Display_Actor()
    {
        handler_setup();
        memset((void*)&local_data, 0, sizeof(local_data));
    }

    friend void core1LaunchThread(void);

public:
    /**
     * @brief Gets the local data packet.
     * @return Reference to the local data packet.
     */
    inline actors_common::Packet& get_local_data() { return local_data; }

    /**
     * @brief Sets the local data packet.
     * @param data The data packet to set.
     */
    inline void set_local_data(const actors_common::Packet& data) {
        this->local_data = data;
    }

    /**
     * @brief Gets the session log file name.
     * @return The session log file name.
     */
    static inline const char* get_session_log_file_name() {
        return FILE_SESSION_DATA;
    }

    /**
     * @brief Provides a singleton instance of Display_Actor.
     * @return Reference to the singleton instance.
     */
    static inline Display_Actor& get_instance()
    {
        static Display_Actor* this_p = nullptr;
        if (this_p == nullptr)
        {
            this_p = new Display_Actor();
        }

        return *this_p;
    }
};

#endif // CORE1_H
