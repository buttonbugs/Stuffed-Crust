#ifndef ESP32_SPIN_CONTROL
#define ESP32_SPIN_CONTROL

#include "watchdog.h"

extern float current_frequency;
extern float accel_mount_radius_cm;

void load_melty_config_settings();
void high_speed_set_motor();

#endif