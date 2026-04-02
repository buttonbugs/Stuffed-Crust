#include "melty_config.h"

float throttle_ratio = 0.0;
float forback_ratio = 0.0;
float leftright_ratio = 0.0;
float revolution_ratio = 0.0;
bool facing_up = true;

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