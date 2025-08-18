#pragma once
// #-------------------------------#
// |           includes            |
// #-------------------------------#
// pico includes

// c/c++ includes


// my includes

// #-------------------------------#
// |            macros             |
// #-------------------------------#

// #-------------------------------#
// | global types declarations     |
// #-------------------------------#
template<typename T>
class Filter_Complementary {
public:
    Filter_Complementary(T alpha);

    // Update methods for each sensor
    void update_gps(T gps_altitude);
    void update_baro(T baro_altitude);

    // Get current estimate
    T get_altitude() const { return altitude_estimate; }

    // Parameter getters/setters
    void set_alpha(T new_alpha) { alpha = new_alpha; }
    T get_alpha() const { return alpha; }

    // Reset the filter
    void reset(T initial_altitude = 0);

private:
    T alpha;              // Smoothing factor [0,1]
    T altitude_estimate;  // Current altitude estimate
    T last_baro;         // Last barometer reading
    bool has_baro;       // Flag to track if we have received baro data
    bool has_gps;       // Flag to track if we have received gps data


public:
    Filter_Complementary() : Filter_Complementary(0.98) {}
};

template<typename T>
Filter_Complementary<T>::Filter_Complementary(T alpha)
    : alpha(alpha)
    , altitude_estimate(0)
    , last_baro(0)
    , has_baro(false)
    , has_gps(false)
{
    if (alpha < 0) alpha = 0;
    if (alpha > 1) alpha = 1;
}

template<typename T>
void Filter_Complementary<T>::update_gps(T gps_altitude) {
    if (!has_gps) {
        altitude_estimate = gps_altitude;
        has_gps = true;  // Mark that we have received GPS data
        return;
    }

    // Complementary filter update with GPS (low-frequency component)
    altitude_estimate = (1 - alpha) * gps_altitude + alpha * altitude_estimate;
}

template<typename T>
void Filter_Complementary<T>::update_baro(T baro_altitude) {
    if (!has_baro) {
        // First barometer reading
        last_baro = baro_altitude;
        has_baro = true;
        return;
    }

    // Calculate barometer altitude change
    const T baro_delta = baro_altitude - last_baro;
    last_baro = baro_altitude;

    // Update estimate with barometer change (high-frequency component)
    altitude_estimate += baro_delta;
}

template<typename T>
void Filter_Complementary<T>::reset(T initial_altitude) {
    altitude_estimate = initial_altitude;
    last_baro = 0;
    has_baro = false;
    has_gps = false;
}

// #-------------------------------#
// | global variables declarations |
// #-------------------------------#

// #-------------------------------#
// | global function declarations  |
// #-------------------------------#

