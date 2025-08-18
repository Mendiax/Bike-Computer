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
class Filter_Alpha_Beta {
public:
    Filter_Alpha_Beta(T alpha = 0.1, T beta = 0.005);

    void update(T measurement, T dt);
    void reset(T initial_position = 0, T initial_velocity = 0);

    T get_position() const { return x_k; }
    T get_velocity() const { return v_k; }

    void set_alpha(T new_alpha) { alpha = new_alpha; }
    void set_beta(T new_beta) { beta = new_beta; }

    T get_alpha() const { return alpha; }
    T get_beta() const { return beta; }

private:
    T alpha;  // Position correction coefficient
    T beta;   // Velocity correction coefficient

    T x_k;    // Position estimate
    T v_k;    // Velocity estimate

public:
    Filter_Alpha_Beta() : Filter_Alpha_Beta(0.1, 0.005) {}
};

template<typename T>
Filter_Alpha_Beta<T>::Filter_Alpha_Beta(T alpha, T beta)
    : alpha(alpha)
    , beta(beta)
    , x_k(0)
    , v_k(0)
{}

template<typename T>
void Filter_Alpha_Beta<T>::update(T measurement, T dt) {
    if (dt <= 0) return;  // Avoid division by zero

    // Prediction step
    T x_k_pred = x_k + v_k * dt;
    T v_k_pred = v_k;

    // Update step
    T residual = measurement - x_k_pred;
    x_k = x_k_pred + alpha * residual;
    v_k = v_k_pred + (beta * residual) / dt;
}

template<typename T>
void Filter_Alpha_Beta<T>::reset(T initial_position, T initial_velocity) {
    x_k = initial_position;
    v_k = initial_velocity;
}

// #-------------------------------#
// | global variables declarations |
// #-------------------------------#

// #-------------------------------#
// | global function declarations  |
// #-------------------------------#

