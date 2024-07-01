#ifndef SESSION_HPP
#define SESSION_HPP

#include "common_types.h"
#include "csv_interface.hpp"
#include <string.h>

#define SESSION_DATA_CSV_LEN 100

/**
 * @brief Contains data about the current session.
 *
 * This class manages the data for a session, including start and end times,
 * speed data, and session status. It also provides CSV interface methods
 * for header and line generation.
 */
class Session_Data : public Csv_Interface
{
public:
    uint16_t id;              ///< Session ID
    TimeS time_start;         ///< Start time of the session
    TimeS time_end;           ///< End time of the session
    SpeedData speed;          ///< Speed data for the session

    /**
     * @brief Status of the session.
     */
    enum class Status {
        NOT_STARTED, ///< Session has not started
        PAUSED,      ///< Session is paused
        RUNNING,     ///< Session is running
        ENDED        ///< Session has ended
    };

private:
    Status status;                     ///< Current status of the session
    absolute_time_t absolute_time_start;      ///< Absolute start time
    absolute_time_t absolute_time_last_stop;  ///< Absolute last stop time

    /**
     * @brief Resets the session data.
     */
    inline void reset()
    {
        memset(&this->speed, 0, sizeof(this->speed));
        memset(&this->time_start, 0, sizeof(this->time_start));
        memset(&this->time_end, 0, sizeof(this->time_end));
        this->status = Status::NOT_STARTED;
    }

public:
    /**
     * @brief Constructs a new Session_Data object.
     */
    Session_Data();

    /**
     * @brief Constructs a new Session_Data object from a CSV line.
     * @param csv_line A CSV line representing session data.
     */
    Session_Data(const char* csv_line);

    /**
     * @brief Gets the current status of the session.
     * @return The current status.
     */
    inline Status get_status() const { return this->status; }

    /**
     * @brief Sets the session ID.
     * @param id The session ID.
     */
    inline void set_id(uint16_t id) { this->id = id; }

    /**
     * @brief Checks if the session is currently running.
     * @return True if the session is running, false otherwise.
     */
    inline bool is_running() {
        return this->status == Status::RUNNING;
    }

    /**
     * @brief Checks if the session has started.
     * @return True if the session has started, false otherwise.
     */
    inline bool has_started() {
        return this->status == Status::RUNNING || this->status == Status::PAUSED;
    }

    /**
     * @brief Gets the start time of the session.
     * @return The start time.
     */
    inline TimeS get_start_time() const { return this->time_start; }

    /**
     * @brief Starts the session.
     * @param time The start time.
     */
    void start(TimeS time);

    /**
     * @brief Pauses the session.
     */
    void pause();

    /**
     * @brief Continues the session.
     */
    void cont();

    /**
     * @brief Ends the session.
     * @param time The end time.
     */
    void end(TimeS time);

    /**
     * @brief Updates the session with the latest speed and distance data.
     * @param speed_kph Speed in kilometers per hour.
     * @param distance_m Distance in meters.
     */
    void update(float speed_kph, float distance_m);

    // Csv_Interface methods
    /**
     * @brief Gets the CSV header for the session data.
     * @return The CSV header as a C-style string.
     */
    const char* get_header() override;

    /**
     * @brief Gets a CSV line representing the session data.
     * @return The CSV line as a std::string.
     */
    std::string get_line() override;
};

#endif // SESSION_HPP
