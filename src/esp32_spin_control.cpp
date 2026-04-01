#include <Arduino.h>

#include "esp32_rc_handler.h"
#include "esp32_led_driver.h"
#include "accel_handler.h"
#include "watchdog.h"

#include "melty_config.h"
#include "config_storage.h"

/* 
    Purpose of this file:
        NOT TO use any delay() or while loop in high_speed_set_motor()

    Usage:
        put high_speed_set_motor() inside the loop() in main.cpp
 */

#define gravitational_acceleration 9.8      // m / s^2

uint32_t last_measurement_micros = 0;       // microseconds, the value of micros() when setting the last last_measurement_frequency
float last_measurement_frequency = 0.0;     // Hz (or revolutions per second) of the spin
float robot_direction = 0.0;

static float accel_mount_radius_cm = DEFAULT_ACCEL_MOUNT_RADIUS_CM;
static float accel_zero_g_offset = DEFAULT_ACCEL_ZERO_G_OFFSET_X;
static float led_offset_percent = DEFAULT_LED_OFFSET_PERCENT;         // stored in EEPROM as an INT - but handled as a float for configuration purposes

// loads settings from EEPROM
void load_melty_config_settings_esp32() {       // Avoid multiple definition of `load_melty_config_settings()`
#ifdef ENABLE_EEPROM_STORAGE
    accel_mount_radius_cm = load_accel_mount_radius();
    accel_zero_g_offset = load_accel_zero_g_offset();
    led_offset_percent = load_heading_led_offset();
#endif
}

void init_high_speed_control() {
    load_melty_config_settings();
    last_measurement_micros = micros();  // uint32_t overflows in 70 minutes.
}

void high_speed_set_motor() {
    // Watchdog is hungry
    service_watchdog();

    /* robot direction calculation - in SI units */
    float radial_acceleration = fabs(get_accel_force_g() - accel_zero_g_offset) * gravitational_acceleration;
    float angular_velocity = sqrt(radial_acceleration / (accel_mount_radius_cm * 0.01f));       // radial_acceleration = angular_velocity^2 * radius
    float current_frequency = angular_velocity / (2.0f * PI);                                   // angular_velocity = 2 * PI * frequency

    /* Accumulate frequency to get robot direction */
    float interval = (micros() - last_measurement_micros) * 0.000001f;                          // convert to seconds
    last_measurement_micros = micros();
    robot_direction += (current_frequency + last_measurement_frequency) * interval / 2.0f;      // trapezoid estimation
    if (robot_direction > 1.0f) {
        robot_direction -= 1.0f;
    }
    
    // Update LED
    update_led(robot_direction);

    // Update Motor
}