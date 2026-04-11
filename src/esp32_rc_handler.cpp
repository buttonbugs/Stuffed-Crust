#include "melty_config.h"
#ifndef ENABLE_ESP32_RC
    #include "rc_handler.h"
#endif

float throttle_ratio = 0.0;             // from 0.0 to 1.0 based on stick position
float forback_ratio = 0.0;              // from -1.0 to 1.0 based on stick position
float leftright_ratio = 0.0;            // from -1.0 to 1.0 based on stick position
float revolution_ratio = 0.0;           // from -1.0 to 1.0 based on stick position
bool facing_up = true;
bool config_mode = false;

// https://github.com/ricardoquesada/bluepad32?tab=readme-ov-file
// set throttle_ratio from 0.0 to 1.0 based on stick position
void esp32_rc_throttle_callback() {
    // Set the variable throttle_ratio
}

// set forback_ratio from -1.0 to 1.0 based on stick position
void esp32_rc_forback_callback() {
    // Set the variable forback_ratio
}

// set leftright_ratio from -1.0 to 1.0 based on stick position
void esp32_rc_leftright_callback() {
    // Set the variable leftright_ratio
}

// set revolution_ratio from -1.0 to 1.0 based on stick position
void esp32_rc_revolution_callback() {
    // Set the variable revolution_ratio
}

// set facing_up ture/false based on the switch status on the xbox controller
void esp32_rc_facing_callback() {
    // Set the variable facing_up
}

// set config_mode ture/false based on the switch status on the xbox controller
void esp32_rc_configmode_callback() {
    // Set the variable config_mode
}

void get_data_from_external_rc() {
    #ifndef ENABLE_ESP32_RC
        throttle_ratio = rc_get_throttle_percent() / 100.0f;
        forback_ratio = rc_get_forback_ratio();
        leftright_ratio = rc_get_leftright_ratio();
        revolution_ratio = rc_get_revolution_ratio();
    #endif
}