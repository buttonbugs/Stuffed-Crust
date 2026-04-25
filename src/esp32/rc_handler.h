#ifndef ESP32_RC_HANDLER
#define ESP32_RC_HANDLER

// Teensy compatibility enum
typedef enum {
    RC_FORBACK_FORWARD = 1,
    RC_FORBACK_NEUTRAL = 0,
    RC_FORBACK_BACKWARD = -1
} rc_forback;

// Bluepad32-based RC handler for Xbox controller input

extern float throttle_ratio;            // 0.0 to 1.0 (triggers)
extern float forback_ratio;             // -1.0 to 1.0 (left stick Y)
extern float leftright_ratio;           // -1.0 to 1.0 (left stick X)
extern float revolution_ratio;          // -1.0 to 1.0 (right stick X)
extern bool facing_up;                  // true/false (A button)
extern bool config_mode;                // true/false (B button)
extern bool config_left;                // Config mode decreasing radius
extern bool config_right;               // Config mode increasing radius
extern bool config_heading;             // Config mode increasing led_offset_ratio

void init_rc();                         // Initialize Bluepad32
void service_rc();                      // Called regularly to update input
bool rc_signal_is_healthy();            // Return true if controller connected
float rc_get_throttle_percent();        // 0-100%
float rc_get_forback_ratio();           // -1.0 to 1.0
float rc_get_leftright_ratio();         // -1.0 to 1.0
float rc_get_revolution_ratio();        // -1.0 to 1.0
bool rc_get_config_mode();
bool rc_get_facing_direction();

// ===== Teensy Compatibility Functions =====
rc_forback rc_get_forback();            // Returns RC_FORBACK_FORWARD/NEUTRAL/BACKWARD
rc_forback rc_get_leftright();          // Returns RC_FORBACK_FORWARD/NEUTRAL/BACKWARD
bool get_config_mode();                 // Returns config mode state

// RC timeout constant (for compatibility with main.cpp)
#define MAX_MS_BETWEEN_RC_UPDATES 900   // If gamepad data not received within this time, signal is unhealthy

#endif